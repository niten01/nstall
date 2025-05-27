#pragma once
#include <array>
#include <cstdint>
#include <sodium.h>

namespace nstall {

/// Appended to target binary and aims to provide reader with security and
/// payload structure related information
///
/// To be safe from unlikely (but possible) order changes there is some
/// redundancy and repetition in member data (i. e. zipSize == nameOffset)
struct Footer {
  std::array<std::byte, 8> magic;
  uint32_t payloadOffset; //< Offset from binary begining
  uint32_t zipOffset;     //< Offset from payload begining
  uint32_t zipSize;
  uint32_t nameOffset;
  uint32_t nameSize;
  std::array<std::byte, crypto_generichash_BYTES>
      checksum; //< Should be last
};

} // namespace nstall
