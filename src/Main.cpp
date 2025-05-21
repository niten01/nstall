#include "nstall/PayloadExtractor.hpp"
#include <filesystem>
#include <nana/gui.hpp>
#include <spdlog/spdlog.h>

auto main(int argc, char** argv) -> int {
  std::filesystem::path binaryPath{ argv[0] };
  std::filesystem::path tmpDestination{ binaryPath.parent_path() /
                                        "nstall_tmp" };

  spdlog::set_pattern("[%T.%e] %^[%l]%$ %v");
  spdlog::info("Starting nstallation");

  if (!std::filesystem::exists(binaryPath) ||
      !std::filesystem::exists(tmpDestination)) {
    spdlog::error("argv mismatch");
  }

  nstall::PayloadExtractor extractor{ argv[0], tmpDestination };
  return 0;
}
