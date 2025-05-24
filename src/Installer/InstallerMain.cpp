#include "nana/basic_types.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nstall/Installer/Installer.hpp"
#include <filesystem>
#include <nana/gui.hpp>
#include <nana/gui/basis.hpp>
#include <nana/gui/programming_interface.hpp>
#include <system_error>

auto main(int argc, char** argv) -> int {
  nstall::Installer installer{ argv[0] };

  installer.run();

  // nana::form fm{ nana::API::make_center(600, 400) };
  // fm.caption("nstall");
  // nana::button b{ fm, nana::rectangle{ 0, 0, 200, 200 } };
  // fm.show();
  // nana::exec();

  return 0;
}
