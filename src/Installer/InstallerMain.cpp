#include "nstall/Installer/InstallerForm.hpp"
#include <filesystem>
#include <cxxopts.hpp>

auto main(int argc, char** argv) -> int {

  try {
    nstall::InstallerForm installer{ argv[0] };
    installer.run();
  } catch (const nstall::InstallerFormException& e) {
    nana::msgbox err{ "Error" };
    err.icon(nana::msgbox::icon_error);
    err << e.what();
    err();
    return 1;
  }

  return 0;
}
