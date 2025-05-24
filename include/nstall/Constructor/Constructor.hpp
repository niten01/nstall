#pragma once
#include "nana/gui/widgets/button.hpp"
#include "nana/gui/widgets/form.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/progress.hpp"
#include "nana/gui/widgets/textbox.hpp"
#include "nana/threads/pool.hpp"
#include <filesystem>
#include <nana/gui.hpp>

namespace nstall {

struct ConstructorException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct Constructor : public nana::form {
public:
  explicit Constructor(std::filesystem::path resourcesPath);

  void run();

private:
  void enableAll(bool enable);

  void createForm();
  void setupCallbacks();

  void onDirectoryClick();
  void onMainClick();

  void validateFields();

private:
  nana::place mainLayout_;
  nana::label title_;

  nana::label programNameLabel_;
  nana::textbox programNameTextBox_;

  nana::label directoryLabel_;
  nana::textbox directoryTextBox_;
  nana::button directoryButton_;

  nana::button mainButton_;
  nana::progress progress_;
  nana::threads::pool pool_;

  std::filesystem::path resourcesPath_;
};

} // namespace nstall
