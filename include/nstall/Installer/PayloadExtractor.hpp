#pragma once
#include "nstall/Common/Payload.hpp"
#include <filesystem>
#include <fstream>

namespace nstall {

struct PayloadExtractorException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class PayloadExtractor {
public:
  explicit PayloadExtractor(const std::filesystem::path& argv0);

  auto extract() -> std::unique_ptr<Payload>;

private:
  std::ifstream stream_;
};

} // namespace nstall
