#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace nstall {

class Payload {
public:
  Payload(std::string programName, std::vector<std::byte> data);

  [[nodiscard]] auto data() const -> const std::vector<std::byte>&;
  [[nodiscard]] auto programName() const -> const std::string&;

private:
  std::vector<std::byte> data_;
  std::string programName_;
};

} // namespace nstall
