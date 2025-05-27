#pragma once
#include <cxxopts.hpp>
#include <filesystem>
#include <stdexcept>

namespace nstall {

struct ConstructorCLIException : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class ConstructorCLI {
public:
  ConstructorCLI(std::filesystem::path resourcesDir, int argc,
                 char** argv);

  void run();

private:
  void validateOpts();
  void pack();
private:
  cxxopts::Options cliOptions_;
  cxxopts::ParseResult opts_;
  std::filesystem::path resourcesDir_;
  int argc_;
  char** argv_;
  std::filesystem::path directory_;
  std::string programName_;
  std::filesystem::path outFile_;
};

} // namespace nstall
