#include "nstall/Installer/PayloadExtractor.hpp"
#include "nstall/Common/Footer.hpp"
#include "nstall/Common/MetaInfo.hpp"
#include "nstall/Common/Utils.hpp"
#include <cstddef>
#include <ios>
#include <iosfwd>
#include <memory>
#include <sodium.h>

using namespace nstall;
namespace fs = std::filesystem;

PayloadExtractor::PayloadExtractor(const std::filesystem::path& argv0)
    : stream_{ argv0, std::ios::binary | std::ios::ate },
      footer_{} {
  if (!stream_.good()) {
    throw PayloadExtractorException{ "Failed to open file" };
  }
}

void PayloadExtractor::install(const fs::path& dstDir) {
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

  stream_.seekg(static_cast<std::streamoff>(sizeof(Footer)),
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

  return std::make_unique<MetaInfo>(std::move(programName));
}
