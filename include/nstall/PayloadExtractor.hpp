#pragma once
#include <filesystem>
#include <fstream>

namespace nstall {

class PayloadExtractor {
public:
  PayloadExtractor(const std::filesystem::path& argv0,
                            const std::filesystem::path& destination);

private:
  std::ifstream m_Stream;
};

} // namespace nstall
