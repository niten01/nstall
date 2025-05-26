#include "nstall/Constructor/ConstructorForm.hpp"
#include <filesystem>
#include <nana/gui.hpp>

auto main(int /*argc*/, char const* argv[]) -> int {
  std::filesystem::path argv0{ argv[0] };
  auto resourcesDir = argv0.parent_path() / "resources";

  try {
    nstall::ConstructorForm constructor{ resourcesDir };
    constructor.run();
  } catch (const nstall::ConstructorException& e) {
    nana::msgbox err{ "Error" };
    err.icon(nana::msgbox::icon_error);
    err << e.what();
    err();
    return 1;
  }
}
