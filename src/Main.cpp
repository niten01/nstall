#include "nstall/Installer.hpp"
#include "nstall/PayloadExtractor.hpp"
#include <filesystem>
#include <nana/gui.hpp>
#include <nana/gui/basis.hpp>
#include <nana/gui/programming_interface.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <system_error>

auto main(int argc, char** argv) -> int {
  spdlog::set_pattern("[%T.%e] %^[%l]%$ %v");
  spdlog::set_level(spdlog::level::debug);
  spdlog::info("Starting nstallation");

  nstall::Installer installer{ argv[0] };
  std::error_code FSEC;
  std::filesystem::path binaryPath{ argv[0] };
  std::filesystem::path tmpDestination{ binaryPath.parent_path() /
                                        "nstall_tmp" };


  std::filesystem::create_directories(tmpDestination, FSEC);
  if (FSEC.default_error_condition()) {
    spdlog::error("Failed to create temporary directory");
  }

  if (!std::filesystem::exists(binaryPath)) {
    spdlog::error("argv mismatch");
  }

  nstall::PayloadExtractor extractor{ argv[0], tmpDestination };

  return 0;
}
