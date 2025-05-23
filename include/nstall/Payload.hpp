#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace nstall {

class Payload {
public:
  Payload(std::string programName, std::vector<std::byte> data)
      : m_Data(std::move(data)),
        m_ProgramName(std::move(programName)) {
  }

  [[nodiscard]] auto data() const -> const std::vector<std::byte>& {
    return m_Data;
  }
  [[nodiscard]] auto programName() const -> const std::string& {
    return m_ProgramName;
  }

private:
  std::vector<std::byte> m_Data;
  std::string m_ProgramName;
};

} // namespace nstall
