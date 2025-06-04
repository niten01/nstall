#include "nstall/Installer/InstallerForm.hpp"
#include "nana/basic_types.hpp"
#include "nana/gui/basis.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nana/threads/pool.hpp"
#include "nstall/Common/Theme.hpp"
#include "nstall/Installer/PayloadExtractor.hpp"
#include <cassert>
#include <filesystem>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <nana/gui/filebox.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/programming_interface.hpp>
#include <nana/gui/widgets/label.hpp>
#include <utility>

using namespace nstall;
namespace fs = std::filesystem;

InstallerForm::InstallerForm(fs::path argv0)
    : nana::form{ nana::API::make_center(300, 300),
                  nana::appear::decorate<nana::appear::taskbar>{} },
      argv0_{ std::move(argv0) } {
  try {
    extractor_ = std::make_unique<PayloadExtractor>(argv0_);
    metaInfo_  = extractor_->verify();
  } catch (const PayloadExtractorException& e) {
    throw InstallerFormException{ e.what() };
  }
  createForm();
}

void InstallerForm::createForm() {
  mainLayout_.bind(*this);

  mainLayout_.div(R"(
  <weight=10>
  <vertical
    <weight=10>
    <title weight=15%> 
    <welcome> 
    <vertical
      <vertical weight=50
        <destination_label>
        <
          <destination_textbox>
          <destination_btn weight=50>
        >
      >
      <>
    >
    <weight=10%
      <switchable
        <main_btn>
        <progress>
      >
    >
    <weight=10>
  >
  <weight=10>
  )");

  theme::Styler styler{};
  styler.font("Segoe UI");
  title_.create(*this);
  title_.text_align(nana::align::center);
  title_.typeface(nana::paint::font{
      styler.font(), 20, nana::paint::font::font_style{ 500 } });
  title_.caption(metaInfo_->programName());
  styler.stylize(title_);
  mainLayout_["title"] << title_;

  welcomeLabel_.create(*this);
  welcomeLabel_.caption(fmt::format(
      "This program will install \"{}\" to your computer. Please choose "
      "destination directory below.",
      metaInfo_->programName()));
  welcomeLabel_.typeface(nana::paint::font{
      styler.font(), 12, nana::paint::font::font_style{ 300 } });
  styler.stylize(welcomeLabel_);
  mainLayout_["welcome"] << welcomeLabel_;

  destinationLabel_.create(*this);
  destinationLabel_.caption("Install to:");
  styler.stylize(destinationLabel_);
  destinationTextBox_.create(*this);
  destinationTextBox_.editable(true).multi_lines(false);
  styler.stylize(destinationTextBox_);
  destinationButton_.create(*this);
  destinationButton_.caption("...");
  styler.stylize(destinationButton_);
  mainLayout_["destination_label"] << destinationLabel_;
  mainLayout_["destination_textbox"] << destinationTextBox_;
  mainLayout_["destination_btn"] << destinationButton_;

  mainButton_.create(*this);
  mainButton_.caption("Install");
  styler.stylize(mainButton_);
  mainLayout_["main_btn"] << mainButton_;

  progress_.create(*this);
  progress_.amount(1000);
  progress_.unknown(false);
  styler.stylize(progress_);
  mainLayout_["progress"] << progress_;

  mainLayout_.collocate();

  styler.stylize(*this);
  typeface(nana::paint::font{ "Consolas", 12 });
  caption("Installer");
  show();

  setupCallbacks();
}

void InstallerForm::run() {
  nana::exec();
}

void InstallerForm::setupCallbacks() {
  destinationButton_.events().click([this] {
    onDestinationButtonClick();
  });
  mainButton_.events().click(nana::threads::pool_push(
      pool_, *this, &InstallerForm::onMainClick));
}

void InstallerForm::onDestinationButtonClick() {
  nana::folderbox fb{ *this, fs::current_path(), "Select destination" };
  fb.allow_multi_select(false);
  auto paths{ fb.show() };

  if (paths.empty()) {
    return;
  }

  assert(paths.size() == 1);

  std::cerr << metaInfo_->programDirName() << std::endl;
  auto destination{ paths.front() / metaInfo_->programDirName() };
  destinationTextBox_.reset(destination.string(), true);
}

void InstallerForm::onMainClick() {
  lockForm(true);

  try {
    verifyFields();
    fs::path destination{ destinationTextBox_.text() };
    fs::create_directories(destination.parent_path());
    extractor_->setProgressCallback(
        [this](auto&& status, float progress) {
          progress_.value(progress * progress_.amount());
          title_.caption(std::string{ status });
        });
    extractor_->install(destination.parent_path());
  } catch (const InstallerFormException& e) {
    nana::msgbox mb{ *this, "Error" };
    mb.icon(nana::msgbox::icon_error);
    mb << e.what();
    mb.show();
    lockForm(false);
    return;
  } catch (const PayloadExtractorException& e) {
    nana::msgbox mb{ *this, "Extractor error" };
    mb.icon(nana::msgbox::icon_error);
    mb << "Extraction error:\n" << e.what();
    mb.show();
    lockForm(false);
    return;
  } catch (const fs::filesystem_error& e) {
    nana::msgbox mb{ *this, "OS Error" };
    mb.icon(nana::msgbox::icon_error);
    mb << "Filesystem error:\n" << e.what();
    mb.show();
    lockForm(false);
    return;
  }

  lockForm(false);

  nana::msgbox mb{ *this, "Success" };
  mb.icon(nana::msgbox::icon_information);
  mb << fmt::format("\"{}\" installed successfully",
                    metaInfo_->programName());
  mb.show();
}

void InstallerForm::enableAll(bool enable) {
  destinationButton_.enabled(enable);
  destinationTextBox_.enabled(enable);
  mainButton_.enabled(enable);
}

void InstallerForm::verifyFields() {
  if (destinationTextBox_.empty()) {
    throw InstallerFormException{ "Destination required" };
  }
}

void InstallerForm::lockForm(bool lock) {
  static std::string ogTitle;

  if (lock) {
    ogTitle = title_.caption();
    mainLayout_.field_display("progress", true);
  } else {
    mainLayout_.field_display("main_btn", true);
    title_.caption(ogTitle);
  }

  enableAll(!lock);
  mainLayout_.collocate();
}
