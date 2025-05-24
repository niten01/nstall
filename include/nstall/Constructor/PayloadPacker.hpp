#pragma once
#include "nstall/Common/Payload.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>

namespace nstall {

struct PayloadPackerException : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class PayloadPacker {
public:
  PayloadPacker(std::filesystem::path carrierPath,
                std::filesystem::path targetPath,
                std::filesystem::path dataDir, std::string programName);

  void pack();
  void setProgressCallback(
      const std::function<void(std::string_view, float)>& cb);

private:
  void createOffsettedZip();
  void injectExecutable();
  void finalizePayload();
  void attachFooter();

private:
  size_t bytesToCompress_;
  size_t carrierSize_;
  size_t zipSize_;
  std::filesystem::path carrierPath_;
  std::filesystem::path targetPath_;
  std::filesystem::path dataDir_;
  std::string programName_;
  std::fstream targetStream_;
  std::function<void(std::string_view, float)> progressCallback_;
};

} // namespace nstall
