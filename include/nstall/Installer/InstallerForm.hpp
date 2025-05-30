#pragma once
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/widgets/textbox.hpp"
#include "nana/threads/pool.hpp"
#include "nstall/Common/MetaInfo.hpp"
#include "nstall/Installer/PayloadExtractor.hpp"
#include <filesystem>
#include <memory>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>

namespace nstall {

struct InstallerFormException : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class InstallerForm : public nana::form {

public:
  explicit InstallerForm(std::filesystem::path argv0);

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

  std::unique_ptr<MetaInfo> metaInfo_;
  std::unique_ptr<PayloadExtractor> extractor_;

  std::filesystem::path tmpDirectory_;
  std::filesystem::path argv0_;

  static constexpr auto tmpDirectoryName_{ "nstall_tmp" };
};

} // namespace nstall
