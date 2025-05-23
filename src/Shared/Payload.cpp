#include <nstall/Shared/Payload.hpp>

using namespace nstall;

Payload::Payload(std::string programName, std::vector<std::byte> data)
  : m_Data(std::move(data)),
    m_ProgramName(std::move(programName)) {}

auto nstall::Payload::data() const -> const std::vector<std::byte>& {
  return m_Data;
}

auto nstall::Payload::programName() const -> const std::string& {
  return m_ProgramName;
}
