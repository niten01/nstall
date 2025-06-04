#pragma once
#include "nana/paint/graphics.hpp"
#include <nana/gui.hpp>
#include <string_view>

namespace nstall::theme {

class Styler {
public:
  Styler()
      : bgColor_{ "#1f1f1f" },
        fgColor_{ nana::colors::white } {
    font("Consolas");
  }

  template <typename T>
  void stylize(T& widget) {
    colorize(widget);
  }

  [[nodiscard]] auto font() const -> const std::string& {
    return font_;
  }
  void font(std::string font) {
    font_ = std::move(font);
    nana::paint::font defaultFont{ font_, 12 };
    defaultFont.set_default();
  }

private:
  template <typename T>
  void colorize(T& widget) {
    widget.bgcolor(bgColor_);
    widget.fgcolor(fgColor_);
  }

private:
  nana::color bgColor_;
  nana::color fgColor_;
  std::string font_;
};

}; // namespace nstall::theme
