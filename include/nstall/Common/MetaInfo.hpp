#pragma once
#include <string>

namespace nstall {

class MetaInfo {
public:
  explicit MetaInfo( std::string programName);

  [[nodiscard]] auto programName() const -> const std::string&;

private:
  std::string programName_;
};

} // namespace nstall
