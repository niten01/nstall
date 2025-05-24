#pragma once
#include "nana/paint/graphics.hpp"
#include <nana/gui.hpp>

namespace nstall::theme {

inline nana::color bgColor;
inline nana::color fgColor;

namespace detail {
inline bool initialized = false;
} // namespace detail

inline void initStyle() {
  bgColor = nana::color{ "#1f1f1f" };
  fgColor = nana::colors::white;

  nana::paint::font defaultFont{ "Consolas", 12 };
  defaultFont.set_default();
}

template <typename T>
void colorize(T& widget) {
  if (!detail::initialized) {
    initStyle();
    detail::initialized = true;
  }
  widget.bgcolor(bgColor);
  widget.fgcolor(fgColor);
}

template <typename T>
void stylize(T& widget) {
  colorize(widget);
}

}; // namespace nstall::theme
