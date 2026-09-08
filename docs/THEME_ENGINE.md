# Theme Engine

`ui_theme_t` in `src/include/theme.h` centralizes wallpaper, window, panel,
sidebar, text, border, control, selection, alpha, shadow, and radius values.
Runtime presets are RatanaOS Dark, RatanaOS Light, macOS Dark, macOS Light,
and RTC-based Auto. Icon selection is intentionally not part of `ui_theme_t`.

Public GUI aliases are `ui_theme_set`, `ui_theme_get`, `ui_theme_next`, and
`ui_theme_get_name`. Existing `theme_*` APIs remain valid. Accent changes use
the existing named color set. Auto resolves from CMOS time during theme update.
