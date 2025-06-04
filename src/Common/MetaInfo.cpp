#include <nstall/Common/MetaInfo.hpp>

using namespace nstall;

MetaInfo::MetaInfo(std::string programName, std::string programDirName)
    : programName_{ std::move(programName) },
      programDirName_{ std::move(programDirName) } {
}

auto nstall::MetaInfo::programName() const -> const std::string& {
  return programName_;
}
auto MetaInfo::programDirName() const -> const std::string& {
  return programDirName_;
}
