#include "nstall/Constructor/PayloadPacker.hpp"
#include "nstall/Common/Footer.hpp"
#include "sodium/crypto_generichash.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <ios>
#include <iostream>
#include <miniz.h>
#include <miniz_zip.h>
#include <sodium.h>
#include <string_view>

using namespace nstall;
namespace fs = std::filesystem;

namespace {

constexpr size_t chunkSize{ 64UL * 1024 }; // 64KB

void handleMzError(mz_zip_archive& zip, auto status) {
  if (!status) {
    auto err{ mz_zip_get_last_error(&zip) };
    std::string mzErrStr = mz_zip_get_error_string(err);
    if (err == MZ_ZIP_FILE_STAT_FAILED ||
        err == MZ_ZIP_FILE_OPEN_FAILED) {
      mzErrStr = "Could not open file, check permissions.\n(zip error: " +
                 mzErrStr + ")";
    }
    throw PayloadPackerException{ mzErrStr };
  }
}

/// @todo this might need parallelization
void writeChunked(std::fstream& dst, std::ifstream& src, size_t len) {
  std::array<std::byte, chunkSize> buffer{};
  while (len) {
    auto toRead =
        static_cast<std::streamsize>(std::min(len, buffer.size()));
    src.read(reinterpret_cast<char*>(buffer.data()), toRead);  // NOLINT
    dst.write(reinterpret_cast<char*>(buffer.data()), toRead); // NOLINT
    len -= toRead;
  }
}

} // namespace

PayloadPacker::PayloadPacker(std::filesystem::path carrierPath,
                             std::filesystem::path targetPath,
                             std::filesystem::path dataDir,
                             std::string programName)
    : zipSize_{ 0 },
      carrierPath_{ std::move(carrierPath) },
      targetPath_{ std::move(targetPath) },
      dataDir_{ std::move(dataDir) },
      programName_{ std::move(programName) },
      progressCallback_{ [](auto&&, auto&&) {
      } } {
  if (!fs::exists(carrierPath_)) {
    throw PayloadPackerException{ "Can't open " + carrierPath_.string() };
  }

  bytesToCompress_ = 0;
  try {
    for (auto&& entry : fs::recursive_directory_iterator(dataDir_)) {
      if (entry.is_directory()) {
        continue;
      }
      bytesToCompress_ += fs::file_size(entry.path());
    }

    carrierSize_ = fs::file_size(carrierPath_);
  } catch (fs::filesystem_error& e) {
    throw PayloadPackerException{
      "OS error on payload packing initialization:\n   " +
      std::string{ e.what() }
    };
  }
}

void nstall::PayloadPacker::pack() {
  try {
    createOffsettedZip();
    injectExecutable();
    finalizePayload();
    attachFooter();
  } catch (fs::filesystem_error& e) {
    throw PayloadPackerException{
      "OS error on payload initialization:\n   " + std::string{ e.what() }
    };
  }
}

void PayloadPacker::setProgressCallback(
    const std::function<void(std::string_view, float)>& cb) {
  progressCallback_ = cb;
}

void PayloadPacker::createOffsettedZip() {
  mz_zip_archive zip{};
  mz_bool status{};
  status =
      mz_zip_writer_init_file(&zip, targetPath_.c_str(), carrierSize_);
  handleMzError(zip, status);

  size_t compressedBytes{ 0 };
  for (auto&& entry : fs::recursive_directory_iterator(dataDir_)) {
    if (entry.is_directory()) {
      continue;
    }

    progressCallback_("Compressing...",
                      static_cast<float>(compressedBytes) /
                          bytesToCompress_); // NOLINT
    auto relPath = fs::relative(entry.path(), dataDir_.parent_path());
    status       = mz_zip_writer_add_file(&zip, relPath.c_str(),
                                          entry.path().c_str(), "", 0,
                                          MZ_BEST_COMPRESSION);
    compressedBytes += fs::file_size(entry.path());
    handleMzError(zip, status);
  }

  status = mz_zip_writer_finalize_archive(&zip);
  handleMzError(zip, status);
  status = mz_zip_writer_end(&zip);
  handleMzError(zip, status);

  zipSize_ = fs::file_size(targetPath_) - carrierSize_;
}

void PayloadPacker::injectExecutable() {
  progressCallback_("Packing...", 1.0F);
  targetStream_.open(targetPath_, std::ios::binary | std::ios::app);
  std::ifstream carrierStream{ carrierPath_, std::ios::binary };
  if (!targetStream_ || !carrierStream) {
    throw PayloadPackerException{ "Failed to open binaries" };
  }

  std::vector<std::byte> buffer;
  targetStream_.seekp(0, std::ios::beg);
  writeChunked(targetStream_, carrierStream, carrierSize_);
}

void PayloadPacker::finalizePayload() {
  targetStream_.seekp(0, std::ios::end);
  targetStream_.write(programName_.data(),
                      static_cast<std::streamsize>(programName_.size()));
}

void PayloadPacker::attachFooter() {
  Footer footer{};
  std::string_view version{ NSTALL_VERSION };
  footer.magic.fill(std::byte{ 0 });
  std::ranges::transform(version, footer.magic.begin(), [](auto&& a) {
    return std::byte{ static_cast<unsigned char>(a) };
  });
  footer.payloadOffset = carrierSize_;
  footer.zipSize       = zipSize_;
  footer.nameSize      = programName_.size();

  // write footer except checksum
  targetStream_.seekp(0, std::ios::end);
  targetStream_.write(reinterpret_cast<char*>(&footer), // NOLINT
                      sizeof(footer) - sizeof(footer.checksum));

  auto totalSize{ static_cast<size_t>(targetStream_.tellp()) };
  targetStream_.seekp(0, std::ios::beg);

  crypto_generichash_state state{};
  crypto_generichash_init(&state, nullptr, 0, footer.checksum.size());

  std::array<std::byte, chunkSize> buffer{};
  while (totalSize) {
    auto toRead =
        static_cast<std::streamsize>(std::min(totalSize, buffer.size()));
    targetStream_.read(reinterpret_cast<char*>(buffer.data()), // NOLINT
                       toRead);
    crypto_generichash_update(
        &state, reinterpret_cast<unsigned char*>(buffer.data()), // NOLINT
        toRead);
    totalSize -= toRead;
  }
  crypto_generichash_final(
      &state,
      reinterpret_cast<unsigned char*>(footer.checksum.data()), // NOLINT
      footer.checksum.size());

  targetStream_.seekp(0, std::ios::end);
  targetStream_.write(reinterpret_cast<char*>(&footer.checksum), // NOLINT
                      footer.checksum.size());
}
