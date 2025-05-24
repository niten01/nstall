#include "nstall/Constructor/Constructor.hpp"
#include <filesystem>
#include <nana/gui.hpp>

auto main(int /*argc*/, char const* argv[]) -> int {
  std::filesystem::path argv0{ argv[0] };
  auto resourcesDir = argv0.parent_path() / "resources";

  try {
    nstall::Constructor constructor{ resourcesDir };
    constructor.run();
    nana::exec();
  } catch (const nstall::ConstructorException& e) {
    nana::msgbox err{ "Error" };
    err.icon(nana::msgbox::icon_error);
    err << e.what();
    err();
    return 1;
  }
}
