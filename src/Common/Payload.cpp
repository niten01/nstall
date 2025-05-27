#include <nstall/Common/MetaInfo.hpp>

using namespace nstall;

MetaInfo::MetaInfo(std::string programName)
    : programName_{ std::move(programName) } {
}

auto nstall::MetaInfo::programName() const -> const std::string& {
  return programName_;
}
