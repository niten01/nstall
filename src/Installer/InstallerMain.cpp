#include "nstall/Installer/InstallerCLI.hpp"
#ifndef NSTALL_CLI_ONLY
#include "nstall/Installer/InstallerForm.hpp"
#define FORCE_CLI false
#else
#define FORCE_CLI true
#endif
#include <cxxopts.hpp>
#include <filesystem>
#include <fmt/color.h>
#include <iostream>

auto main(int argc, char** argv) -> int {

  if (argc > 1 || FORCE_CLI) {
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

#ifndef NSTALL_CLI_ONLY
  try {
    nstall::InstallerForm installer{ argv[0] };
    installer.run();
  } catch (const nstall::InstallerFormException& e) {
    std::cerr << e.what() << std::endl;
    nana::msgbox err{ "Error" };
    err.icon(nana::msgbox::icon_error);
    err << e.what();
    err();
    return 1;
  }
#endif

  return 0;
}
