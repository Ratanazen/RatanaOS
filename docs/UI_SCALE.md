# UI Scale

The global integer scale supports 80, 90, 100, 110, 125, 150, 175, and 200
percent step navigation. `UI(x)` expands a design-space metric using integer
math. Existing APIs remain `ui_scale_set`, `ui_scale_increase`, and
`ui_scale_decrease`; aliases `ui_scale_up`, `ui_scale_down`, and
`ui_scale_value` are provided for GUI clients.

Dock geometry and existing controls use the scale helper. Some legacy app
content in `gui.c` still has fixed illustrative geometry; expanding those views
should be incremental to preserve their current layout at 1024x768.
