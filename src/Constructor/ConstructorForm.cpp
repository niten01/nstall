#include "nstall/Constructor/ConstructorForm.hpp"
#include "nana/basic_types.hpp"
#include "nana/gui/basis.hpp"
#include "nana/gui/detail/event_code.hpp"
#include "nana/gui/detail/general_events.hpp"
#include "nana/gui/filebox.hpp"
#include "nana/gui/programming_interface.hpp"
#include "nana/paint/graphics.hpp"
#include "nana/threads/pool.hpp"
#include "nstall/Common/Theme.hpp"
#include "nstall/Common/Utils.hpp"
#include "nstall/Constructor/PayloadPacker.hpp"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <nana/gui.hpp>
#include <optional>
#include <ranges>

using namespace nstall;
namespace fs = std::filesystem;

ConstructorForm::ConstructorForm(std::filesystem::path resourcesPath)
    : nana::form{ nana::API::make_center(400, 300),
                  nana::appearance{ true, false, true, false, true, false,
                                    false } },
      resourcesPath_{ std::move(resourcesPath) } {
  if (!fs::exists(resourcesPath_) || !fs::is_directory(resourcesPath_)) {
    throw ConstructorFormException{
      resourcesPath_.string() + " not found. Check Nstall installation"
    };
  }
  createForm();
}

void ConstructorForm::run() {
  nana::exec();
}

void ConstructorForm::createForm() {
  mainLayout_.bind(*this);

  mainLayout_.div(R"(
  <weight=10>
  <vertical
    <weight=10>
    <title weight=20%> 
    <vertical
      <vertical weight=50
        <program_name_label>
        <program_name_textbox>
      >
      <weight=20>
      <vertical weight=50
        <directory_label>
        <
          <directory_textbox>
          <directory_btn weight=50>
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

  title_.create(*this);
  title_.typeface(
      nana::paint::font{ "", 20, nana::paint::font::font_style{ 500 } });
  title_.text_align(nana::align::center);
  title_.caption("Create your Nstaller");
  theme::stylize(title_);
  mainLayout_["title"] << title_;

  programNameLabel_.create(*this);
  programNameLabel_.caption("Program name:");
  theme::stylize(programNameLabel_);
  programNameTextBox_.create(*this);
  programNameTextBox_.editable(true).multi_lines(false);
  theme::stylize(programNameTextBox_);
  mainLayout_["program_name_label"] << programNameLabel_;
  mainLayout_["program_name_textbox"] << programNameTextBox_;

  directoryLabel_.create(*this);
  directoryLabel_.caption("Program shipping directory:");
  theme::stylize(directoryLabel_);
  directoryTextBox_.create(*this);
  directoryTextBox_.editable(true).multi_lines(false);
  theme::stylize(directoryTextBox_);
  directoryButton_.create(*this);
  directoryButton_.caption("...");
  theme::stylize(directoryButton_);
  mainLayout_["directory_label"] << directoryLabel_;
  mainLayout_["directory_textbox"] << directoryTextBox_;
  mainLayout_["directory_btn"] << directoryButton_;

  mainButton_.create(*this);
  mainButton_.caption("Create");
  theme::stylize(mainButton_);
  mainLayout_["main_btn"] << mainButton_;

  progress_.create(*this);
  progress_.amount(1000);
  progress_.unknown(false);
  mainLayout_["progress"] << progress_;

  mainLayout_.collocate();
  theme::stylize(*this);
  typeface(nana::paint::font{ "Consolas", 12 });
  caption("Nstaller Constructor");
  show();

  setupCallbacks();
}

void nstall::ConstructorForm::validateFields() {
  if (programNameTextBox_.text().empty()) {
    throw ConstructorFormException{ "Program name required" };
  }

  if (directoryTextBox_.text().empty()) {
    throw ConstructorFormException{ "Program directory required" };
  }

  if (!fs::exists(directoryTextBox_.text())) {
    throw ConstructorFormException{ directoryTextBox_.text() +
                                    " not found" };
  }

  if (!fs::is_directory(directoryTextBox_.text())) {
    throw ConstructorFormException{
      "Program directory must be a directory"
    };
  }

  if (fs::is_empty(directoryTextBox_.text())) {
    throw ConstructorFormException{ directoryTextBox_.text() +
                                    " is empty" };
  }
}

void ConstructorForm::setupCallbacks() {
  directoryButton_.events().click([this] {
    onDirectoryClick();
  });
  mainButton_.events().click(nana::threads::pool_push(
      pool_, *this, &ConstructorForm::onMainClick));
}

void ConstructorForm::onDirectoryClick() {
  nana::folderbox fb{ *this, fs::current_path(),
                      "Select program shipping directory" };
  fb.allow_multi_select(false);
  auto paths{ fb.show() };

  if (paths.empty()) {
    return;
  }

  assert(paths.size() == 1);

  directoryTextBox_.reset(paths[0].string(), true);
}

void ConstructorForm::onMainClick() {
  std::string programName{ programNameTextBox_.text() };
  std::string programNameSafe{ utils::safeFilename(programName) };
  fs::path directory{ directoryTextBox_.text() };

  nana::filebox fb{ *this, false };
  fb.init_file(fs::current_path() /
               (programNameSafe + "_Installer" + NSTALL_EXE_EXTENSION));
  fb.add_filter("Executable", "*." NSTALL_EXE_EXTENSION);
  fb.allow_multi_select(false);
  auto paths{ fb.show() };
  if (paths.empty()) {
    return;
  }

  assert(paths.size() == 1);
  const auto& targetPath{ paths[0] };

  auto ogTitle{ title_.caption() };
  enableAll(false);
  mainLayout_.field_display("progress", true);
  mainLayout_.collocate();

  try {
    validateFields();
    auto carrierPathOpt{ utils::findFileByStem(resourcesPath_,
                                        NSTALL_CARRIER_NAME) };
    if (!carrierPathOpt) {
      throw ConstructorFormException{
        "Carrier file not found, check Nstall installation"
      };
    }

    PayloadPacker payloadPacker{ *carrierPathOpt, targetPath, directory,
                                 programName };
    payloadPacker.setProgressCallback(
        [this](auto&& status, float progress) {
          progress_.value(progress * progress_.amount());
          title_.caption(std::string{ status });
        });
    payloadPacker.pack();
  } catch (ConstructorFormException& e) {
    nana::msgbox mb{ *this, "Error" };
    mb.icon(nana::msgbox::icon_error);
    mb << e.what();
    mb.show();
    return;
  } catch (PayloadPackerException& e) {
    nana::msgbox mb{ *this, "Packing error" };
    mb.icon(nana::msgbox::icon_error);
    mb << "Packing error:\n" << e.what();
    mb.show();
    return;
  } catch (fs::filesystem_error& e) {
    nana::msgbox mb{ *this, "OS Error" };
    mb.icon(nana::msgbox::icon_error);
    mb << "Filesystem error:\n" << e.what();
    mb.show();
    return;
  }

  mainLayout_.field_display("main_btn", true);
  title_.caption(ogTitle);
  enableAll(true);
  mainLayout_.collocate();

  nana::msgbox mb{ *this, "Success" };
  mb.icon(nana::msgbox::icon_information);
  mb << "Installer created.\nLocation: " << targetPath;
  mb.show();
}

void ConstructorForm::enableAll(bool enable) {
  programNameTextBox_.enabled(enable);
  directoryTextBox_.enabled(enable);
  directoryButton_.enabled(enable);
  mainButton_.enabled(enable);
}
