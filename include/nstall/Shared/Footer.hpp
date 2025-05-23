#pragma once
#include <array>
#include <cstdint>

namespace nstall {

struct Footer {
  static constexpr auto maxProgramNameSize{ 64 };

  uint32_t magic;
  uint32_t payloadOffset;
  uint32_t payloadSize;
  uint32_t checksum;
  std::array<char, maxProgramNameSize> programName;
};

} // namespace nstall
