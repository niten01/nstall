#pragma once
#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <ios>
#include <iterator>
#include <miniz.h>
#include <optional>
#include <sodium.h>
#include <string>
#include <unordered_set>

namespace nstall::utils {

template <size_t ChecksumSize, typename Stream>
auto calcChecksum(Stream& stream, size_t srcSize)
    -> std::array<std::byte, ChecksumSize> {
  static constexpr size_t chunkSize{ 64UL * 1024 }; // 64KiB

  crypto_generichash_state state{};
  crypto_generichash_init(&state, nullptr, 0, ChecksumSize);

  std::array<std::byte, chunkSize> buffer{};
  while (srcSize) {
    auto toRead{ static_cast<std::streamsize>(
        std::min(srcSize, buffer.size())) };
    stream.read(reinterpret_cast<char*>(buffer.data()), // NOLINT
                toRead);
    crypto_generichash_update(
        &state, reinterpret_cast<unsigned char*>(buffer.data()), // NOLINT
        toRead);
    srcSize -= toRead;
  }

  std::array<std::byte, ChecksumSize> result{};

  crypto_generichash_final(
      &state,
      reinterpret_cast<unsigned char*>(result.data()), // NOLINT
      result.size());
  return result;
}

template <size_t S>
void fillVersionMagic(std::array<std::byte, S>& magic) {
  std::string_view version{ NSTALL_VERSION };
  magic.fill(std::byte{ 0 });
  std::ranges::transform(version, magic.begin(), [](auto&& a) {
    return std::byte{ static_cast<unsigned char>(a) };
  });
}

inline auto safeFilename(const std::string& filename) -> std::string {
  std::string res{};
  std::ranges::replace_copy(filename, std::back_inserter(res), ' ', '_');

  // replace all bad characters with markers
  static constexpr auto invalidMarker{ '*' };
  std::ranges::transform(res, res.begin(), [](auto&& c) {
    if (std::isalnum(c) ||
        std::unordered_set<char>{ '_', '-', '.' }.contains(c)) {
      return c;
    }
    return invalidMarker;
  });
  std::ranges::remove(res, invalidMarker);
  return res;
}

inline auto findFileByStem(const std::filesystem::path& dir,
                           const std::string& name)
    -> std::optional<std::filesystem::path> {
  namespace fs = std::filesystem;
  for (auto&& entry : fs::directory_iterator{ dir }) {
    if (entry.is_regular_file() && entry.path().stem().string() == name) {
      return entry.path();
    }
  }
  return std::nullopt;
}

template <typename ExceptionT>
void handleMzError(mz_zip_archive& zip, auto status) {
  if (!status) {
    auto err{ mz_zip_get_last_error(&zip) };
    std::string mzErrStr = mz_zip_get_error_string(err);
    if (err == MZ_ZIP_FILE_STAT_FAILED ||
        err == MZ_ZIP_FILE_OPEN_FAILED) {
      mzErrStr = "Could not open file, check permissions.\n(zip error: " +
                 mzErrStr + ")";
    }
    throw ExceptionT{ "zip: " + mzErrStr };
  }
}

} // namespace nstall::utils
