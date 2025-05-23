#include "nstall/PayloadExtractor.hpp"
#include "nstall/Footer.hpp"
#include <bit>
#include <cstddef>
#include <ios>

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

  m_Stream.seekg(static_cast<std::streamoff>(size - sizeof(Footer)));
  Footer footer{};
}
