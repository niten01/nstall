#pragma once
#include <cstdint>

namespace nstall {

struct Footer {
  uint32_t magic;
  uint32_t payloadOffset;
  uint32_t payloadSize;
};


} // namespace nstall
