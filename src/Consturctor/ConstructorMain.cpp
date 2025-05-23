#include <filesystem>

auto main(int  /*argc*/, char const *argv[]) -> int {
  std::filesystem::path argv0{ argv[0] };
  auto resourcesDir = argv0.parent_path()/"resources";
}
