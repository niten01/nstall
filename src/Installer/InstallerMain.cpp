#include "nstall/Installer/InstallerCLI.hpp"
#include "nstall/Installer/InstallerForm.hpp"
#include <cxxopts.hpp>
#include <filesystem>
#include <fmt/color.h>

auto main(int argc, char** argv) -> int {

  if (argc > 1) {
    try {
      nstall::InstallerCLI installer{ argv[0], argc, argv };
      installer.run();
    } catch (const nstall::InstallerCLIException& e) {
      fmt::print(stderr, fmt::fg(fmt::color::indian_red), "[error]: ");
      fmt::println(stderr, "{}\n", e.what());
      return 1;
    }
    return 0;
  }

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
