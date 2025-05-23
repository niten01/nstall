#include "nstall/Installer.hpp"
#include "nana/basic_types.hpp"
#include "nana/gui/basis.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nstall/Footer.hpp"
#include "nstall/PayloadExtractor.hpp"
#include <filesystem>
#include <nana/gui/place.hpp>
#include <nana/gui/programming_interface.hpp>
#include <nana/gui/widgets/label.hpp>
#include <spdlog/fmt/bundled/format.h>
#include <utility>

using namespace nstall;
namespace fs = std::filesystem;

Installer::Installer(fs::path argv0)
    : nana::form{ nana::API::make_center(300, 300),
                  nana::appear::decorate<nana::appear::taskbar>{} },
      m_Argv0{ std::move(argv0) } {
  m_TmpDirectory = fs::temp_directory_path() / m_TmpDirectoryName;
}

void Installer::createForm() {
  m_MainLayout.bind(*this);

  m_MainLayout.div(fmt::format(R"(
    <vertical
      <vertical fit
        <title> 
        <welcome>
      >
      <vertical
        <>
        <vertical weight={destinationHeightPx}
          <destination_label>
          <
            <destination_textbox>
            <destination_btn weight={destinationHeightPx}>
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
  )",
                               fmt::arg("destinationHeightPx", 50)));

  m_Title.create(*this);
  m_Title.text_align(nana::align::center);
  m_Title.caption("Program name");
  m_Title.typeface(
      nana::paint::font{ "", 20, nana::paint::font::font_style{ 500 } });
  m_MainLayout["title"] << m_Title;

  m_WelcomeLabel.create(*this);
  m_WelcomeLabel.caption();
  m_WelcomeLabel.typeface(
      nana::paint::font{ "", 20, nana::paint::font::font_style{ 500 } });
  m_MainLayout["welcome"] << m_WelcomeLabel;

  m_DestinationLabel.create(*this);
  m_DestinationLabel.caption("Install to:");
  m_DestinationTextBox.create(*this);
  m_DestinationTextBox.editable(true).multi_lines(false);
  m_DestinationButton.create(*this);
  m_DestinationButton.caption("...");
  m_MainLayout["destination_label"] << m_DestinationLabel;
  m_MainLayout["destination_textbox"] << m_DestinationTextBox;
  m_MainLayout["destination_btn"] << m_DestinationButton;

  m_MainButton.create(*this);
  m_MainButton.caption("Install");
  m_MainLayout["main_btn"] << m_MainButton;

  m_MainLayout.collocate();

  caption("Installer");
  show();
}

void Installer::run() {
    PayloadExtractor extractor{ m_Argv0};
    m_Payload = extractor.extract();
    createForm();

  try {
    fs::create_directories(m_TmpDirectory);

    nana::exec();
  } catch (const fs::filesystem_error& e) {
    throw InstallerException{ fmt::format(
        "OS error on installer initialization:\n   {}", e.what()) };
  }
}
