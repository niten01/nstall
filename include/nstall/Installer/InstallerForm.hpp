#pragma once
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/widgets/progress.hpp"
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

  void enableAll(bool enable);
  void lockForm(bool lock);
  void verifyFields();

  void setupCallbacks();
  void onDestinationButtonClick();
  void onMainClick();

private:
  nana::place mainLayout_;
  nana::label title_;
  nana::label welcomeLabel_;
  nana::button mainButton_;
  nana::progress progress_;
  nana::label destinationLabel_;
  nana::textbox destinationTextBox_;
  nana::button destinationButton_;
  nana::threads::pool pool_;

  std::unique_ptr<MetaInfo> metaInfo_;
  std::unique_ptr<PayloadExtractor> extractor_;

  std::filesystem::path argv0_;
};

} // namespace nstall
