#include "nstall/Installer/PayloadExtractor.hpp"
#include "fmt/format.h"
#include "miniz_common.h"
#include "nstall/Common/Footer.hpp"
#include "nstall/Common/MetaInfo.hpp"
#include "nstall/Common/Utils.hpp"
#include <chrono>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <memory>
// main miniz header should come first :(
#include <miniz.h>
#include <miniz_tdef.h>
#include <miniz_zip.h>
#include <sodium.h>
#include <thread>

using namespace nstall;
namespace fs = std::filesystem;

PayloadExtractor::PayloadExtractor(const std::filesystem::path& argv0)
    : binaryPath_{ argv0 },
      stream_{ argv0, std::ios::binary | std::ios::ate },
      footer_{},
      progressCallback_{ [](auto, auto) {
      } } {
  if (!stream_.good()) {
    throw PayloadExtractorException{ "Failed to open file" };
  }
}

void PayloadExtractor::install(const fs::path& dstDir) {
  mz_zip_archive zip{};
  mz_bool status{};
  status = mz_zip_reader_init_file(&zip, binaryPath_.string().c_str(), 0);
  utils::handleMzError<PayloadExtractorException>(zip, status);

  try {
    uint32_t numFilesToExtract{ mz_zip_reader_get_num_files(&zip) };
    uint64_t bytesToExtract{ 0 };
    for (uint32_t i = 0; i < numFilesToExtract; i++) {
      mz_zip_archive_file_stat stat{};
      status = mz_zip_reader_file_stat(&zip, i, &stat);
      utils::handleMzError<PayloadExtractorException>(zip, status);

      bytesToExtract += stat.m_comp_size;
    }

    uint64_t bytesExtracted{ 0 };
    for (uint32_t i = 0; i < numFilesToExtract; i++) {
      mz_zip_archive_file_stat stat{};
      status = mz_zip_reader_file_stat(&zip, i, &stat);
      utils::handleMzError<PayloadExtractorException>(zip, status);

      if (stat.m_is_directory) {
        continue;
      }

      fs::path targetPath{ dstDir / stat.m_filename };
      fs::create_directories(targetPath.parent_path());

      progressCallback_("Extracting...",
                        static_cast<float>(bytesExtracted) /
                            static_cast<float>(bytesToExtract));
      status =
          mz_zip_reader_extract_to_file(&zip, i, targetPath.c_str(), 0);
      utils::handleMzError<PayloadExtractorException>(zip, status);
      bytesExtracted += stat.m_comp_size;
    }
  } catch (const fs::filesystem_error& e) {
    throw PayloadExtractorException{ fmt::format("OS error: {}",
                                                 e.what()) };
  }

  mz_zip_reader_end(&zip);
}

auto PayloadExtractor::verify() -> std::unique_ptr<MetaInfo> {
  size_t totalSize = stream_.tellg();
  if (totalSize < sizeof(Footer)) {
    throw PayloadExtractorException{ "File too small" };
  }

  static constexpr size_t checksumSize = sizeof(footer_.checksum);
  stream_.seekg(0, std::ios::beg);
  auto realChecksum{ utils::calcChecksum<checksumSize>(
      stream_, totalSize - checksumSize) };

  stream_.seekg(static_cast<std::streamoff>(-sizeof(Footer)),
                std::ios::end);
  stream_.read(reinterpret_cast<char*>(&footer_), // NOLINT
               sizeof(Footer));

  if (footer_.checksum != realChecksum) {
    throw PayloadExtractorException(
        "Checksums didn't match. Installer may be corrupted");
  }

  std::array<std::byte, sizeof(footer_.magic)> masterMagic{};
  utils::fillVersionMagic(masterMagic);
  if (footer_.magic != masterMagic) {
    throw PayloadExtractorException(
        "Installer corrupted (version mismatch). Contact distributor");
  }

  stream_.seekg(static_cast<std::streampos>(footer_.payloadOffset +
                                            footer_.nameOffset),
                std::ios::beg);
  std::string programName(footer_.nameSize, '\0');
  stream_.read(programName.data(), footer_.nameSize);

  stream_.seekg(static_cast<std::streampos>(footer_.payloadOffset +
                                            footer_.dirNameOffset),
                std::ios::beg);
  std::string programDirName(footer_.dirNameSize, '\0');
  stream_.read(programDirName.data(), footer_.dirNameSize);

  return std::make_unique<MetaInfo>(std::move(programName),
                                    std::move(programDirName));
}

void PayloadExtractor::setProgressCallback(
    const std::function<void(std::string_view, float)>& cb) {
  progressCallback_ = cb;
}
