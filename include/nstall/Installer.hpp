#pragma once
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/widgets/textbox.hpp"
#include "nana/threads/pool.hpp"
#include "nstall/Payload.hpp"
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

  nana::place m_MainLayout;
  nana::label m_Title;
  nana::label m_WelcomeLabel;
  nana::button m_MainButton;
  nana::label m_DestinationLabel;
  nana::textbox m_DestinationTextBox;
  nana::button m_DestinationButton;
  nana::threads::pool m_ThreadPool;

  std::filesystem::path m_TmpDirectory;
  std::filesystem::path m_Argv0;

  std::unique_ptr<Payload> m_Payload;

  static constexpr auto m_TmpDirectoryName{ "nstall_tmp" };
};

} // namespace nstall
