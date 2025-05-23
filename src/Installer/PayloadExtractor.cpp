#include "nstall/PayloadExtractor.hpp"
#include "nstall/Footer.hpp"
#include <bit>
#include <cstddef>
#include <ios>
#include <sodium.h>

using namespace nstall;

PayloadExtractor::PayloadExtractor(const std::filesystem::path& argv0)
    : m_Stream{ argv0, std::ios::binary | std::ios::ate } {
  if (!m_Stream.good()) {
    throw PayloadExtractorException{ "Failed to open file" };
  }
}

auto PayloadExtractor::extract() -> std::unique_ptr<Payload> {
  size_t size = m_Stream.tellg();
  if (size < sizeof(Footer)) {
    throw PayloadExtractorException{ "File too small" };
  }

  size_t endOffset = size;
  static constexpr size_t checksumSize = crypto_generichash_BYTES;
  endOffset -= checksumSize;
  std::array<std::byte, checksumSize> checksum{};
  m_Stream.seekg(static_cast<std::streamoff>(size - endOffset));
  m_Stream.read(reinterpret_cast<char*>(checksum.data()), checksumSize);
  // Footer footer{};
}
