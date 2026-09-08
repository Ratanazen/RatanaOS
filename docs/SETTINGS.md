# Settings

The settings object contains UI theme, independent icon theme, accent, scale,
font size, Dock size/spacing/magnification, window radius, transparency, and
shadows. `settings_apply` applies all values to their current subsystems.

`settings_save` and `settings_load` use a checksum-protected volatile memory
backend. They survive only for the kernel lifetime; no filesystem, ATA, or
NVRAM persistence is implemented. `settings_get_backend_name` and
`settings_is_persistent` expose that limitation explicitly.
