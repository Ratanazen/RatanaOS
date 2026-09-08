# Font System

RatanaOS uses the embedded 8x16 bitmap font and integer raster scaling; it does
not link FreeType or host libraries. Supported raster sizes are Small 6x12,
Regular 8x16, Large 12x24, and Title 16x32. `font_set_size_px` safely maps a
requested pixel size to one of those supported rasters.

Roles are System, UI, Menu, Title, Caption, and Monospace. They are a compact
selection abstraction over the single embedded face, ready for future font
assets without forcing a large dependency into the kernel.
