#pragma once
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/widgets/textbox.hpp"
#include "nana/threads/pool.hpp"
#include "nstall/Common/Payload.hpp"
#include <filesystem>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>

namespace nstall {

struct InstallerException : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class Installer : public nana::form {

public:
  explicit Installer(std::filesystem::path argv0);

  void run();

private:
  void createForm();

  void onBackClick();
  void onNextClick();

  nana::place mainLayout_;
  nana::label title_;
  nana::label welcomeLabel_;
  nana::button mainButton_;
  nana::label destinationLabel_;
  nana::textbox destinationTextBox_;
  nana::button destinationButton_;
  nana::threads::pool threadPool_;

  std::filesystem::path tmpDirectory_;
  std::filesystem::path argv0_;

  std::unique_ptr<Payload> payload_;

  static constexpr auto tmpDirectoryName_{ "nstall_tmp" };
};

} // namespace nstall
