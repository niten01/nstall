#include "nstall/Constructor/ConstructorCLI.hpp"
#include "cxxopts.hpp"
#include "fmt/base.h"
#include "fmt/color.h"
#include "nstall/Common/Utils.hpp"
#include "nstall/Constructor/PayloadPacker.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <iostream>

using namespace nstall;
namespace fs = std::filesystem;

ConstructorCLI::ConstructorCLI(fs::path resourcesDir, int argc,
                               char** argv)
    : cliOptions_{ "nstall-constructor",
                   "Minimalistic installer builder" },
      resourcesDir_{ std::move(resourcesDir) },
      argc_{ argc },
      argv_{ argv } {
  if (!fs::exists(resourcesDir_)) {
    throw ConstructorCLIException{
      resourcesDir_.string() + " not found. Check Nstall installation"
    };
  }

  // clang-format off
  cliOptions_.add_options()
      ("h,help", "Show help")
      ("n,name", "Specify installed program name (required)",
       cxxopts::value<std::string>())
      ("d,directory", "Program shipping directory, may be positional (required)",
       cxxopts::value<std::string>())
      ("o,out", "Where to put created installer",
       cxxopts::value<std::string>()->default_value(fs::current_path()));
  // clang-format on
  cliOptions_.positional_help("<directory>");
  cliOptions_.show_positional_help();
}

void ConstructorCLI::run() {
  try {
    cliOptions_.parse_positional("directory");
    opts_ = cliOptions_.parse(argc_, argv_);
    validateOpts();
    fmt::print(fmt::fg(fmt::color::sky_blue), "[*] ");
    fmt::println("Starting installer packing");
    pack();
    fmt::print(fmt::fg(fmt::color::pale_green), "Success!\n");
    fmt::println("Path to created installer: {}", outFile_.string());
  } catch (const cxxopts::exceptions::parsing& e) {
    throw ConstructorCLIException{ fmt::format("{}\n{}", e.what(),
                                               cliOptions_.help()) };
  } catch (const fs::filesystem_error& e) {
    throw ConstructorCLIException{ e.what() };
  } catch (const PayloadPackerException& e) {
    throw ConstructorCLIException{ fmt::format(
        "Installer packing error:\n{}", e.what()) };
  }
}

void ConstructorCLI::validateOpts() {
  if (opts_.contains("help")) {
    std::cout << cliOptions_.help();
    std::exit(0); // NOLINT
  }

  if (!opts_.contains("name") || !opts_.contains("directory")) {
    throw ConstructorCLIException{ fmt::format(
        "Missing requred flags\n{}", cliOptions_.help()) };
  }

  directory_   = opts_["directory"].as<std::string>();
  programName_ = opts_["name"].as<std::string>();
  outFile_     = fs::path{ opts_["out"].as<std::string>() } /
             utils::safeFilename(programName_ + "_Installer");

  if (!fs::exists(directory_)) {
    throw ConstructorCLIException{ "Could not find shipping directory: " +
                                   directory_.string() };
  }

  fs::create_directories(outFile_.parent_path());
}

void ConstructorCLI::pack() {
  auto carrierPathOpt{ utils::findFileByStem(resourcesDir_,
                                             NSTALL_CARRIER_NAME) };
  if (!carrierPathOpt) {
    throw ConstructorCLIException{
      "Carrier file not found, check Nstall installation"
    };
  }

  PayloadPacker packer{ *carrierPathOpt, outFile_, directory_,
                        programName_ };
  packer.setProgressCallback([](std::string_view status, float progress) {
    static std::string lastStatus{};
    if (lastStatus != status) {
      fmt::print("\n");
    }
    lastStatus = status;
    fmt::print(fmt::fg(fmt::color::pale_green), "[*] ");
    fmt::print("{}: {:.2f}%\r", status, progress * 100.F);
  });
  packer.pack();
  fmt::print("\n");
}
