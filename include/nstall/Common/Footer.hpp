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
  uint64_t payloadOffset; //< Offset from binary begining
  uint64_t zipOffset;     //< Offset from payload begining
  uint64_t zipSize;
  uint64_t nameOffset;
  uint64_t nameSize;
  uint64_t dirNameOffset;
  uint64_t dirNameSize;
  std::array<std::byte, crypto_generichash_BYTES>
      checksum; //< Should be last
};

} // namespace nstall
