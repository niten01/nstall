#include <nstall/Common/Payload.hpp>

using namespace nstall;

Payload::Payload(std::string programName, std::vector<std::byte> data)
  : data_(std::move(data)),
    programName_(std::move(programName)) {}

auto nstall::Payload::data() const -> const std::vector<std::byte>& {
  return data_;
}

auto nstall::Payload::programName() const -> const std::string& {
  return programName_;
}
