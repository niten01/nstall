#include "nstall/Constructor/ConstructorCLI.hpp"
#include <cstdio>
#include <cxxopts.hpp>
#include <filesystem>
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#ifndef NSTALL_CLI_ONLY
#include "nstall/Constructor/ConstructorForm.hpp"
#define FORCE_CLI false
#else
#define FORCE_CLI true
#endif

auto main(int argc, char* argv[]) -> int {
  std::filesystem::path argv0{ argv[0] };
  auto resourcesDir = argv0.parent_path() / "resources";

  // CLI mode
  if (argc > 1 || FORCE_CLI) {
    try {
      nstall::ConstructorCLI constructor{ resourcesDir, argc, argv };
      constructor.run();
    } catch (const nstall::ConstructorCLIException& e) {
      fmt::print(stderr, fmt::fg(fmt::color::indian_red), "[error]: ");
      fmt::println(stderr, "{}\n", e.what());
      return 1;
    }
    return 0;
  }

#ifndef NSTALL_CLI_ONLY
  try {
    nstall::ConstructorForm constructor{ resourcesDir };
    constructor.run();
  } catch (const nstall::ConstructorFormException& e) {
    nana::msgbox err{ "Error" };
    err.icon(nana::msgbox::icon_error);
    err << e.what();
    err();
    return 1;
  }
#endif
}
