#pragma once
#include "nstall/Common/Footer.hpp"
#include "nstall/Common/MetaInfo.hpp"
#include <filesystem>
#include <fstream>
#include <memory>

namespace nstall {

struct PayloadExtractorException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class PayloadExtractor {
public:
  explicit PayloadExtractor(const std::filesystem::path& argv0);

  void install(const std::filesystem::path& dstDir);
  auto verify() -> std::unique_ptr<MetaInfo>;

private:
  std::ifstream stream_;
  Footer footer_;
};

} // namespace nstall
