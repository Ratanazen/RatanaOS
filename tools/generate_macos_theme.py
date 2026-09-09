#!/usr/bin/env python3
import os

THEME_BASE = "ratanaos-live/config/includes.chroot/usr/share/themes"

def make_circle_xpm(color_hex, border_hex, size=16):
    lines = []
    lines.append('/* XPM */')
    lines.append('static char * circle_xpm[] = {')
    lines.append(f'"{size} {size} 3 1",')
    lines.append('"  c None",')
    lines.append(f'". c {border_hex}",')
    lines.append(f'"# c {color_hex}",')
    
    r = size / 2.0 - 1.0
    cx = size / 2.0 - 0.5
    cy = size / 2.0 - 0.5
    
    for y in range(size):
        row = ""
        for x in range(size):
            dist = ((x - cx)**2 + (y - cy)**2)**0.5
            if dist <= r - 1.0:
                row += "#"
            elif dist <= r:
                row += "."
            else:
                row += " "
        lines.append(f'"{row}",')
    lines[-1] = lines[-1].rstrip(',')
    lines.append('};')
    return "\n".join(lines)

def make_title_xpm(bg_hex, height=32, width=4):
    lines = []
    lines.append('/* XPM */')
    lines.append('static char * title_xpm[] = {')
    lines.append(f'"{width} {height} 1 1",')
    lines.append(f'"# c {bg_hex}",')
    for _ in range(height):
        lines.append(f'"{'#' * width}",')
    lines[-1] = lines[-1].rstrip(',')
    lines.append('};')
    return "\n".join(lines)

def generate_theme(theme_name, is_dark=True):
    theme_dir = os.path.join(THEME_BASE, theme_name, "xfwm4")
    os.makedirs(theme_dir, exist_ok=True)
    
    bg_color = "#1E1E24" if is_dark else "#F6F6F7"
    sidebar_bg = "#18181D" if is_dark else "#ECECED"
    inactive_bg = "#25252B" if is_dark else "#E8E8E9"
    fg_color = "#FFFFFF" if is_dark else "#1A1A1A"
    accent_color = "#E0386B" if is_dark else "#007AFF"  # Sequoia magenta accent for dark, Apple blue for light
    
    # macOS traffic light buttons
    red = "#FF5F56"
    red_border = "#E0443E"
    yellow = "#FFBD2E"
    yellow_border = "#DEA123"
    green = "#27C93F"
    green_border = "#1AAB29"
    gray = "#505054" if is_dark else "#C4C4C6"
    
    buttons = {
        'close-active.xpm': make_circle_xpm(red, red_border),
        'close-prelight.xpm': make_circle_xpm("#FF7D75", red_border),
        'close-pressed.xpm': make_circle_xpm("#D83C36", red_border),
        'close-inactive.xpm': make_circle_xpm(gray, "#404044"),
        
        'hide-active.xpm': make_circle_xpm(yellow, yellow_border),
        'hide-prelight.xpm': make_circle_xpm("#FFCE54", yellow_border),
        'hide-pressed.xpm': make_circle_xpm("#C48F1B", yellow_border),
        'hide-inactive.xpm': make_circle_xpm(gray, "#404044"),
        
        'maximize-active.xpm': make_circle_xpm(green, green_border),
        'maximize-prelight.xpm': make_circle_xpm("#48E460", green_border),
        'maximize-pressed.xpm': make_circle_xpm("#189B25", green_border),
        'maximize-inactive.xpm': make_circle_xpm(gray, "#404044"),
    }
    
    for filename, content in buttons.items():
        with open(os.path.join(theme_dir, filename), "w") as f:
            f.write(content)
            
    # Title bars
    title_active = make_title_xpm(bg_color)
    title_inactive = make_title_xpm(inactive_bg)
    
    for prefix in ['title-1', 'title-2', 'title-3', 'title-4', 'title-5', 'top-left', 'top-right']:
        with open(os.path.join(theme_dir, f"{prefix}-active.xpm"), "w") as f:
            f.write(title_active)
        with open(os.path.join(theme_dir, f"{prefix}-inactive.xpm"), "w") as f:
            f.write(title_inactive)
            
    # Borders
    border_color = "#2E2E36" if is_dark else "#DCDCE0"
    border_xpm = make_title_xpm(border_color, height=2, width=2)
    for b in ['bottom-active.xpm', 'bottom-inactive.xpm', 'left-active.xpm', 'left-inactive.xpm', 
              'right-active.xpm', 'right-inactive.xpm', 'bottom-left-active.xpm', 'bottom-left-inactive.xpm',
              'bottom-right-active.xpm', 'bottom-right-inactive.xpm']:
        with open(os.path.join(theme_dir, b), "w") as f:
            f.write(border_xpm)
            
    # themerc
    themerc = f"""# RatanaOS macOS Sequoia 2026 Theme
button_layout=CHM|
button_offset=10
button_spacing=7
title_shadow_active=false
title_shadow_inactive=false
title_alignment=center
active_text_color={fg_color}
inactive_text_color={gray}
full_width_title=true
maximized_offset=0
"""
    with open(os.path.join(theme_dir, "themerc"), "w") as f:
        f.write(themerc)
        
    # GTK-3.0 & GTK-4.0 CSS (Finder & Control Center styling)
    for gtk_ver in ["gtk-3.0", "gtk-4.0"]:
        gtk_dir = os.path.join(THEME_BASE, theme_name, gtk_ver)
        os.makedirs(gtk_dir, exist_ok=True)
        gtk_css = f"""/* RatanaOS macOS Sequoia 2026 {gtk_ver} Theme */
@define-color bg_color {bg_color};
@define-color fg_color {fg_color};
@define-color sidebar_bg {sidebar_bg};
@define-color accent_color {accent_color};
@define-color panel_bg rgba({24 if is_dark else 248}, {24 if is_dark else 248}, {28 if is_dark else 250}, 0.82);

* {{
    font-family: -apple-system, BlinkMacSystemFont, "SF Pro Display", "Cantarell", "Ubuntu", sans-serif;
    -gtk-secondary-caret-color: @accent_color;
}}

window {{
    background-color: @bg_color;
    color: @fg_color;
    border-radius: 12px;
}}

/* macOS Finder Sidebar (Nautilus / Thunar / Dolphin) */
.sidebar, sidebar, placessidebar, .source-list {{
    background-color: @sidebar_bg;
    color: @fg_color;
    border-right: 1px solid rgba(128, 128, 128, 0.15);
}}

.sidebar row, placessidebar row {{
    border-radius: 8px;
    margin: 2px 8px;
    padding: 6px 10px;
    transition: background 0.15s ease-in-out;
}}

.sidebar row:selected, placessidebar row:selected {{
    background-color: alpha(@accent_color, 0.22);
    color: @accent_color;
    font-weight: 600;
}}

/* Headerbars & Toolbars */
headerbar, .titlebar {{
    background: @bg_color;
    border-bottom: 1px solid rgba(128, 128, 128, 0.18);
    min-height: 42px;
    padding: 0 12px;
}}

/* macOS Control Center Popups & Sliders */
.control-center-card, popover.menu, .popup {{
    background-color: rgba({30 if is_dark else 255}, {30 if is_dark else 255}, {36 if is_dark else 255}, 0.88);
    backdrop-filter: blur(24px);
    border-radius: 14px;
    border: 1px solid rgba(255, 255, 255, 0.12);
    box-shadow: 0 12px 32px rgba(0, 0, 0, 0.35);
}}

scale trough {{
    background-color: rgba(128, 128, 128, 0.25);
    border-radius: 10px;
    min-height: 20px;
}}

scale highlight {{
    background: @accent_color;
    border-radius: 10px;
}}

scale slider {{
    background: #FFFFFF;
    border-radius: 50%;
    min-width: 22px;
    min-height: 22px;
    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.3);
}}

/* Top 24px Apple Panel */
.xfce4-panel, #panel {{
    background-color: @panel_bg;
    color: @fg_color;
    font-size: 13px;
    font-weight: 500;
    border-bottom: 1px solid rgba(255, 255, 255, 0.08);
}}

/* Push Buttons */
button {{
    border-radius: 8px;
    padding: 6px 14px;
    background: {'rgba(255, 255, 255, 0.08)' if is_dark else 'rgba(0, 0, 0, 0.05)'};
    color: @fg_color;
    border: 1px solid rgba(128, 128, 128, 0.22);
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.08);
}}

button:hover {{
    background: {'rgba(255, 255, 255, 0.14)' if is_dark else 'rgba(0, 0, 0, 0.08)'};
}}

button.suggested-action {{
    background: @accent_color;
    color: #FFFFFF;
    font-weight: 600;
}}
"""
        with open(os.path.join(gtk_dir, "gtk.css"), "w") as f:
            f.write(gtk_css)
            
    # index.theme
    index_theme = f"""[Desktop Entry]
Type=X-GNOME-Metatheme
Name={theme_name}
Comment=RatanaOS macOS Sequoia 2026 Metatheme
Encoding=UTF-8

[X-GNOME-Metatheme]
GtkTheme={theme_name}
MetacityTheme={theme_name}
IconTheme=RatanaOS-WhiteSur
CursorTheme=Adwaita
ButtonLayout=close,minimize,maximize:
"""
    with open(os.path.join(THEME_BASE, theme_name, "index.theme"), "w") as f:
        f.write(index_theme)

generate_theme("RatanaOS-Dark", is_dark=True)
generate_theme("RatanaOS-Light", is_dark=False)
print("RatanaOS macOS Sequoia 2026 Themes (Dark & Light) generated successfully!")
