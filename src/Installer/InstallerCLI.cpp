#include "nstall/Installer/InstallerCLI.hpp"
#include "fmt/color.h"
#include "fmt/format.h"
#include "nstall/Installer/PayloadExtractor.hpp"
#include <filesystem>
#include <iostream>

using namespace nstall;
namespace fs = std::filesystem;

InstallerCLI::InstallerCLI(const fs::path& argv0, int argc, char** argv)
    : cliOptions_{ "intstaller" },
      argc_{ argc },
      argv_{ argv } {
  // clang-format off
  cliOptions_.add_options()
      ("h,help", "Show help")
      ("d,destination", "Installation directory, may be positional (required)",
       cxxopts::value<std::string>());
  // clang-format on

  cliOptions_.positional_help("<destination>");
  cliOptions_.show_positional_help();
  try {
    extractor_ = std::make_unique<PayloadExtractor>(argv0);
    metaInfo_  = extractor_->verify();
  } catch (const PayloadExtractorException& e) {
    throw InstallerCLIException{ e.what() };
  }
}

void InstallerCLI::run() {
  try {
    cliOptions_.parse_positional("destination");
    opts_ = cliOptions_.parse(argc_, argv_);
    if (opts_.contains("help")) {
      std::cout << cliOptions_.help();
      std::exit(0); // NOLINT
    }

    if (!opts_.contains("destination")) {
      throw InstallerCLIException{ "Destination directory required\n" +
                                   cliOptions_.help() };
    }
    install();
    fmt::print(fmt::fg(fmt::color::pale_green), "\nSuccess!\n");
  } catch (const cxxopts::exceptions::parsing& e) {
    throw InstallerCLIException{ fmt::format("{}\n{}", e.what(),
                                             cliOptions_.help()) };
  } catch (const fs::filesystem_error& e) {
    throw InstallerCLIException{ e.what() };
  } catch (const PayloadExtractorException& e) {
    throw InstallerCLIException{ fmt::format("Extraction error:\n{}",
                                             e.what()) };
  }
}

void InstallerCLI::install() {
  fs::path destination{ opts_["destination"].as<std::string>() };
  fs::create_directories(destination);
  extractor_->setProgressCallback(
      [](std::string_view status, float progress) {
        static std::string lastStatus{};
        if (lastStatus != status) {
          fmt::print("\n");
        }
        lastStatus = status;
        fmt::print(fmt::fg(fmt::color::pale_green), "[*] ");
        fmt::print("{}: {:.2f}%{}\r", status, progress * 100.F,
                   std::string(5, ' '));
      });
  extractor_->install(destination);
}
