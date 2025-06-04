#pragma once
#include "nstall/Common/Footer.hpp"
#include "nstall/Common/MetaInfo.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
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

  void setProgressCallback(
      const std::function<void(std::string_view, float)>& cb);

private:
  std::filesystem::path binaryPath_;
  std::ifstream stream_;
  Footer footer_;
  std::function<void(std::string_view, float)> progressCallback_;
};

} // namespace nstall
