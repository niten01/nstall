#pragma once
#include <array>
#include <cstdint>
#include <sodium.h>

namespace nstall {

struct Footer {
  std::array<std::byte, 8> magic;
  uint32_t payloadOffset;
  uint32_t zipSize;
  uint32_t nameSize;
  std::array<std::byte, crypto_generichash_BYTES>
      checksum; //< should be last
};

} // namespace nstall
