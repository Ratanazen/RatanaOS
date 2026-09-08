# GUI Architecture

The desktop is a retained-mode controller in `src/kernel/gui.c`. It composes a
cached wallpaper, desktop icons, windows in z-order, a menu bar, dock, and
cursor into the software backbuffer once per frame. A static window array avoids
render-loop allocation. Window identity is immutable (`window_t.id`); focus can
reorder z-order without changing application identity.

The existing widget helpers in `ui_controls.c` draw labels, buttons, sidebars,
checkboxes, toggles, sliders, text fields, segmented controls, and lists. They
consume `ui_theme_t`, font sizing, and `UI(...)` scale values.

Input is currently polling based: PS/2 drivers update keyboard/mouse state,
then the GUI routes a click to menu bar, dock, desktop, or the topmost window.
This is deliberately kept compatible with the current kernel event model.
