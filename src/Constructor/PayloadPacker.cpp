#include "nstall/Constructor/PayloadPacker.hpp"
#include "nstall/Common/Footer.hpp"
#include "nstall/Common/Utils.hpp"
#include "sodium/crypto_generichash.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <ios>
#include <iostream>
// main miniz header should come first :(
#include <miniz.h>
#include <miniz_zip.h>
#include <sodium.h>
#include <string_view>

using namespace nstall;
namespace fs = std::filesystem;

namespace {

constexpr size_t chunkSize{ 64UL * 1024 }; // 64KiB

/// @todo this might need parallelization
void writeChunked(std::fstream& dst, std::ifstream& src, size_t len) {
  std::array<std::byte, chunkSize> buffer{};
  while (len) {
    auto toRead =
        static_cast<std::streamsize>(std::min(len, buffer.size()));
    src.read(reinterpret_cast<char*>(buffer.data()), toRead); // NOLINT
    // std::cerr << std::to_integer<char>(buffer[0])
    //           << std::to_integer<char>(buffer[1])
    //           << std::to_integer<char>(buffer[2]) << '\n';
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
      sourceDir_{ std::move(dataDir) },
      programName_{ std::move(programName) },
      progressCallback_{ [](auto&&, auto&&) {
      } } {
  if (!fs::exists(carrierPath_)) {
    throw PayloadPackerException{ "Can't open " + carrierPath_.string() };
  }

  bytesToCompress_ = 0;
  try {
    for (auto&& entry : fs::recursive_directory_iterator(sourceDir_)) {
      if (entry.is_directory()) {
        continue;
      }
      bytesToCompress_ += fs::file_size(entry.path());
    }

    if (sourceDir_.is_relative()) {
      sourceDir_ = fs::absolute(sourceDir_);
    }

    dirNameStr_ = sourceDir_.stem();

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
    mz_zip_archive zip{};
    mz_bool status{};
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
  utils::handleMzError<PayloadPackerException>(zip, status);

  size_t compressedBytes{ 0 };
  for (auto&& entry : fs::recursive_directory_iterator(sourceDir_)) {
    if (entry.is_directory()) {
      continue;
    }

    progressCallback_("Compressing...",
                      static_cast<float>(compressedBytes) /
                          bytesToCompress_); // NOLINT
    auto relPath{ fs::relative(entry.path(), sourceDir_.parent_path()) };
    status = mz_zip_writer_add_file(&zip, relPath.c_str(),
                                    entry.path().c_str(), nullptr, 0,
                                    MZ_UBER_COMPRESSION);
    compressedBytes += fs::file_size(entry.path());
    utils::handleMzError<PayloadPackerException>(zip, status);
  }

  status = mz_zip_writer_finalize_archive(&zip);
  utils::handleMzError<PayloadPackerException>(zip, status);
  status = mz_zip_writer_end(&zip);
  utils::handleMzError<PayloadPackerException>(zip, status);
  zipSize_ = fs::file_size(targetPath_) - carrierSize_;

  // status = mz_zip_reader_init_file(&zip, "a_Installer", 0);
  // utils::handleMzError<PayloadPackerException>(zip, status);
  // std::cout << mz_zip_reader_get_num_files(&zip);
  // mz_zip_archive_file_stat stat{};
  // status = mz_zip_reader_file_stat(&zip, 4, &stat);
  // utils::handleMzError<PayloadPackerException>(zip, status);
  // std::cerr << stat.m_filename << ' ' << stat.m_comment << std::endl;

  fs::permissions(targetPath_, fs::perms::all); // maybe rwxr-xr-x?
}

void PayloadPacker::injectExecutable() {
  progressCallback_("Packing...", 1.0F);
  targetStream_.open(targetPath_,
                     std::ios::binary | std::ios::in | std::ios::out);
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
  targetStream_.write(dirNameStr_.data(),
                      static_cast<std::streamsize>(dirNameStr_.size()));
}

void PayloadPacker::attachFooter() {
  Footer footer{};
  utils::fillVersionMagic(footer.magic);
  footer.payloadOffset = carrierSize_;
  footer.zipOffset     = 0;
  footer.zipSize       = zipSize_;
  footer.nameOffset    = zipSize_;
  footer.nameSize      = programName_.size();
  footer.dirNameOffset = footer.nameOffset + footer.nameSize;
  footer.dirNameSize   = dirNameStr_.size();

  // write footer except checksum
  targetStream_.seekp(0, std::ios::end);
  targetStream_.write(reinterpret_cast<char*>(&footer), // NOLINT
                      sizeof(footer) - sizeof(footer.checksum));

  auto totalSize{ static_cast<size_t>(targetStream_.tellp()) };
  targetStream_.seekp(0, std::ios::beg);

  footer.checksum = utils::calcChecksum<footer.checksum.size()>(
      targetStream_, totalSize);

  targetStream_.seekp(0, std::ios::end);
  targetStream_.write(reinterpret_cast<char*>(&footer.checksum), // NOLINT
                      footer.checksum.size());
}
