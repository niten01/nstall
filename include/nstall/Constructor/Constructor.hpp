#pragma once
#include "nana/gui/widgets/button.hpp"
#include "nana/gui/widgets/form.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/textbox.hpp"
#include <nana/gui.hpp>

namespace nstall {

struct Constructor : public nana::form {
  public:

  private:
  nana::place m_MainLayout;
  nana::label m_Title;
  nana::label m_ProgramNameLabel;
  nana::textbox m_ProgramNameTextBox;

  nana::label m_DirectoryLabel;
  nana::textbox m_DirectoryTextBox;
  nana::button m_DirectoryButton;
};

} // namespace nstall
