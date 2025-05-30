#include "nstall/Installer/InstallerForm.hpp"
#include "nana/basic_types.hpp"
#include "nana/gui/basis.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nstall/Installer/PayloadExtractor.hpp"
#include <filesystem>
#include <memory>
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
  tmpDirectory_ = fs::temp_directory_path() / tmpDirectoryName_;

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
  <vertical
    <vertical fit
      <title> 
      <welcome>
    >
    <vertical
      <>
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
      <>
      <main_btn weight=50%>
      <>
    >
  >
  )");

  title_.create(*this);
  title_.text_align(nana::align::center);
  title_.typeface(nana::paint::font{
      "Consolas", 20, nana::paint::font::font_style{ 500 } });
  title_.caption("Program name");
  mainLayout_["title"] << title_;

  welcomeLabel_.create(*this);
  welcomeLabel_.caption();
  welcomeLabel_.typeface(
      nana::paint::font{ "", 20, nana::paint::font::font_style{ 500 } });
  mainLayout_["welcome"] << welcomeLabel_;

  destinationLabel_.create(*this);
  destinationLabel_.caption("Install to:");
  destinationTextBox_.create(*this);
  fs::path defaultPath{ fs::current_path().root_directory() /
                        metaInfo_->programName() };
  destinationTextBox_.editable(true).multi_lines(false).reset(
      defaultPath.string());
  destinationButton_.create(*this);
  destinationButton_.caption("...");
  mainLayout_["destination_label"] << destinationLabel_;
  mainLayout_["destination_textbox"] << destinationTextBox_;
  mainLayout_["destination_btn"] << destinationButton_;

  mainButton_.create(*this);
  mainButton_.caption("Install");
  mainLayout_["main_btn"] << mainButton_;

  mainLayout_.collocate();

  typeface(nana::paint::font{ "Consolas", 12 });
  caption("Installer");
  show();
}

void InstallerForm::run() {
  nana::exec();
}
