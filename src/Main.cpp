#include "nana/basic_types.hpp"
#include "nana/gui/widgets/button.hpp"
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

  installer.run();

  // nana::form fm{ nana::API::make_center(600, 400) };
  // fm.caption("nstall");
  // nana::button b{ fm, nana::rectangle{ 0, 0, 200, 200 } };
  // fm.show();
  // nana::exec();

  return 0;
}
