#pragma once
#include <string>

namespace nstall {

class MetaInfo {
public:
   MetaInfo( std::string programName,std::string programDirName);

  [[nodiscard]] auto programName() const -> const std::string&;
  [[nodiscard]] auto programDirName() const -> const std::string&;

private:
  std::string programName_;
  std::string programDirName_;
};

} // namespace nstall
