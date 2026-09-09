#!/usr/bin/env python3
import os

THEME_BASE = "ratanaos-live/config/includes.chroot/usr/share/themes"

def make_circle_xpm(color_hex, border_hex, size=16):
    lines = []
    lines.append(f'/* XPM */')
    lines.append(f'static char * circle_xpm[] = {{')
    lines.append(f'"{size} {size} 3 1",')
    lines.append(f'"  c None",')
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

def make_title_xpm(bg_hex, height=28, width=4):
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

def generate_xfwm_theme(theme_name, is_dark=True):
    theme_dir = os.path.join(THEME_BASE, theme_name, "xfwm4")
    os.makedirs(theme_dir, exist_ok=True)
    
    bg_color = "#1E1E1E" if is_dark else "#E8E8E8"
    inactive_bg = "#2A2A2A" if is_dark else "#D8D8D8"
    fg_color = "#FFFFFF" if is_dark else "#1A1A1A"
    
    # Buttons
    # Red: #FF5F56, Yellow: #FFBD2E, Green: #27C93F
    red = "#FF5F56"
    red_border = "#E0443E"
    yellow = "#FFBD2E"
    yellow_border = "#DEA123"
    green = "#27C93F"
    green_border = "#1AAB29"
    gray = "#606060" if is_dark else "#B0B0B0"
    
    buttons = {
        'close-active.xpm': make_circle_xpm(red, red_border),
        'close-prelight.xpm': make_circle_xpm("#FF7D75", red_border),
        'close-pressed.xpm': make_circle_xpm("#D83C36", red_border),
        'close-inactive.xpm': make_circle_xpm(gray, "#404040"),
        
        'hide-active.xpm': make_circle_xpm(yellow, yellow_border),
        'hide-prelight.xpm': make_circle_xpm("#FFCE54", yellow_border),
        'hide-pressed.xpm': make_circle_xpm("#C48F1B", yellow_border),
        'hide-inactive.xpm': make_circle_xpm(gray, "#404040"),
        
        'maximize-active.xpm': make_circle_xpm(green, green_border),
        'maximize-prelight.xpm': make_circle_xpm("#48E460", green_border),
        'maximize-pressed.xpm': make_circle_xpm("#189B25", green_border),
        'maximize-inactive.xpm': make_circle_xpm(gray, "#404040"),
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
    border_color = "#333333" if is_dark else "#CCCCCC"
    border_xpm = make_title_xpm(border_color, height=2, width=2)
    for b in ['bottom-active.xpm', 'bottom-inactive.xpm', 'left-active.xpm', 'left-inactive.xpm', 
              'right-active.xpm', 'right-inactive.xpm', 'bottom-left-active.xpm', 'bottom-left-inactive.xpm',
              'bottom-right-active.xpm', 'bottom-right-inactive.xpm']:
        with open(os.path.join(theme_dir, b), "w") as f:
            f.write(border_xpm)
            
    # themerc: layout = CHM| (Close, Hide/Min, Max on left!)
    themerc = f"""# RatanaOS macOS Sequoia XFWM4 Theme
button_layout=CHM|
button_offset=8
button_spacing=6
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
        
    # GTK-3.0 CSS
    gtk_dir = os.path.join(THEME_BASE, theme_name, "gtk-3.0")
    os.makedirs(gtk_dir, exist_ok=True)
    gtk_css = f"""/* RatanaOS macOS Sequoia GTK 3 Theme */
@define-color bg_color {bg_color};
@define-color fg_color {fg_color};
@define-color accent_color #007AFF;
@define-color panel_bg rgba({30 if is_dark else 240}, {30 if is_dark else 240}, {30 if is_dark else 240}, 0.85);

* {{
    border-radius: 6px;
    font-family: -apple-system, BlinkMacSystemFont, "SF Pro Display", "Ubuntu", sans-serif;
}}

window {{
    background-color: @bg_color;
    color: @fg_color;
}}

headerbar {{
    background-color: @bg_color;
    border-bottom: 1px solid rgba(128, 128, 128, 0.2);
    min-height: 38px;
}}

.xfce4-panel {{
    background-color: @panel_bg;
    color: @fg_color;
    border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}}

button {{
    border-radius: 6px;
    padding: 4px 10px;
    background: {'#2A2A2A' if is_dark else '#F0F0F0'};
    color: @fg_color;
    border: 1px solid rgba(128, 128, 128, 0.3);
}}

button:hover {{
    background: {'#383838' if is_dark else '#E4E4E4'};
}}

button.suggested-action {{
    background: @accent_color;
    color: white;
}}
"""
    with open(os.path.join(gtk_dir, "gtk.css"), "w") as f:
        f.write(gtk_css)
        
    # index.theme
    index_theme = f"""[Desktop Entry]
Type=X-GNOME-Metatheme
Name={theme_name}
Comment=RatanaOS macOS Sequoia Theme
Encoding=UTF-8

[X-GNOME-Metatheme]
GtkTheme={theme_name}
MetacityTheme={theme_name}
IconTheme=WhiteSur
CursorTheme=Adwaita
ButtonLayout=close,minimize,maximize:
"""
    with open(os.path.join(THEME_BASE, theme_name, "index.theme"), "w") as f:
        f.write(index_theme)

generate_xfwm_theme("RatanaOS-Dark", is_dark=True)
generate_xfwm_theme("RatanaOS-Light", is_dark=False)
print("Themes RatanaOS-Dark and RatanaOS-Light successfully created!")
