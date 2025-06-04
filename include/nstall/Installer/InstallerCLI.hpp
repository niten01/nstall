#pragma once
#include "cxxopts.hpp"
#include "nstall/Common/MetaInfo.hpp"
#include "nstall/Installer/PayloadExtractor.hpp"
#include <filesystem>
#include <memory>
#include <stdexcept>

namespace nstall {

struct InstallerCLIException : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class InstallerCLI {
public:
  explicit InstallerCLI(const std::filesystem::path& argv0, int argc, char** argv);

  void run();

private:
  void install();

private:
  int argc_;
  char** argv_;
  cxxopts::Options cliOptions_;
  cxxopts::ParseResult opts_;
  std::unique_ptr<MetaInfo> metaInfo_;
  std::unique_ptr<PayloadExtractor> extractor_;
};

} // namespace nstall
