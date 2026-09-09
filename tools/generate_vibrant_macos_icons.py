#!/usr/bin/env python3
"""
Generates high-fidelity macOS Tahoe & Sequoia Squircle Icons:
- Full color gradient squircles (smooth superellipse)
- Subtle top highlight reflection & soft ambient drop shadow
- Embedded crisp vector glyphs matching Apple's Human Interface Guidelines
"""
import os
import math
from PIL import Image, ImageDraw, ImageFont, ImageFilter

TARGET_BASE = "ratanaos-live/config/includes.chroot/usr/share/icons"
THEMES = ["RatanaOS-WhiteSur", "RatanaOS-MacTahoe"]
SIZES = [16, 22, 24, 32, 48, 64, 128, 256]
CANVAS = 512

def create_squircle_mask(size=CANVAS, radius=110):
    mask = Image.new("L", (size, size), 0)
    draw = ImageDraw.Draw(mask)
    draw.rounded_rectangle([(32, 32), (size - 32, size - 32)], radius=radius, fill=255)
    return mask

def create_gradient_squircle(top_color, bottom_color, size=CANVAS):
    im = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    grad = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    gdraw = ImageDraw.Draw(grad)
    for y in range(size):
        r = int(top_color[0] + (bottom_color[0] - top_color[0]) * (y / size))
        g = int(top_color[1] + (bottom_color[1] - top_color[1]) * (y / size))
        b = int(top_color[2] + (bottom_color[2] - top_color[2]) * (y / size))
        gdraw.line([(0, y), (size, y)], fill=(r, g, b, 255))
    
    mask = create_squircle_mask(size)
    im.paste(grad, (0, 0), mask)
    
    # Inner border / glass edge
    draw = ImageDraw.Draw(im)
    draw.rounded_rectangle([(32, 32), (size - 32, size - 32)], radius=110, outline=(255, 255, 255, 60), width=3)
    return im

def add_shadow(img):
    shadow = Image.new("RGBA", (CANVAS + 64, CANVAS + 64), (0, 0, 0, 0))
    sdraw = ImageDraw.Draw(shadow)
    sdraw.rounded_rectangle([(64, 68), (CANVAS + 32, CANVAS + 36)], radius=120, fill=(0, 0, 0, 75))
    shadow = shadow.filter(ImageFilter.GaussianBlur(radius=18))
    shadow.paste(img, (32, 20), img)
    return shadow.resize((CANVAS, CANVAS), Image.Resampling.LANCZOS)

def draw_finder_icon():
    base = create_gradient_squircle((100, 195, 255), (0, 110, 240))
    draw = ImageDraw.Draw(base)
    # Split two-tone face
    # Left half lighter
    draw.rounded_rectangle([(32, 32), (CANVAS // 2, CANVAS - 32)], radius=110, fill=(130, 215, 255, 70))
    # Smile line and eyes
    # Left Eye
    draw.ellipse([(160, 170), (195, 225)], fill=(20, 60, 120, 255))
    # Right Eye
    draw.ellipse([(317, 170), (352, 225)], fill=(20, 60, 120, 255))
    # Nose & Smile
    draw.line([(CANVAS//2, 180), (CANVAS//2, 290)], fill=(20, 60, 120, 255), width=16)
    draw.arc([(150, 250), (362, 380)], start=20, end=160, fill=(20, 60, 120, 255), width=18)
    return add_shadow(base)

def draw_safari_icon():
    base = create_gradient_squircle((255, 255, 255), (230, 235, 245))
    draw = ImageDraw.Draw(base)
    # Blue compass circle
    draw.ellipse([(80, 80), (CANVAS - 80, CANVAS - 80)], fill=(0, 122, 255, 255), outline=(255, 255, 255, 180), width=6)
    # Compass tick marks
    for deg in range(0, 360, 30):
        rad = math.radians(deg)
        cx, cy = CANVAS // 2, CANVAS // 2
        x1 = cx + int(math.cos(rad) * 155)
        y1 = cy + int(math.sin(rad) * 155)
        x2 = cx + int(math.cos(rad) * 170)
        y2 = cy + int(math.sin(rad) * 170)
        draw.line([(x1, y1), (x2, y2)], fill=(255, 255, 255, 220), width=4)
    # Red/White needle
    needle_red = [(CANVAS//2, CANVAS//2 - 130), (CANVAS//2 + 25, CANVAS//2), (CANVAS//2 - 25, CANVAS//2)]
    needle_white = [(CANVAS//2, CANVAS//2 + 130), (CANVAS//2 + 25, CANVAS//2), (CANVAS//2 - 25, CANVAS//2)]
    # Rotate needle by -45 degrees
    def rot(pt, angle=-45):
        cx, cy = CANVAS // 2, CANVAS // 2
        rad = math.radians(angle)
        x, y = pt
        nx = cx + (x - cx) * math.cos(rad) - (y - cy) * math.sin(rad)
        ny = cy + (x - cx) * math.sin(rad) + (y - cy) * math.cos(rad)
        return (nx, ny)
    draw.polygon([rot(p) for p in needle_red], fill=(255, 59, 48, 255))
    draw.polygon([rot(p) for p in needle_white], fill=(245, 245, 247, 255))
    draw.ellipse([(CANVAS//2 - 12, CANVAS//2 - 12), (CANVAS//2 + 12, CANVAS//2 + 12)], fill=(255, 255, 255, 255))
    return add_shadow(base)

def draw_music_icon():
    base = create_gradient_squircle((255, 60, 95), (250, 20, 60))
    draw = ImageDraw.Draw(base)
    # White Double Musical Note
    draw.ellipse([(140, 300), (220, 370)], fill=(255, 255, 255, 255))
    draw.ellipse([(280, 260), (360, 330)], fill=(255, 255, 255, 255))
    draw.rectangle([(200, 150), (220, 335)], fill=(255, 255, 255, 255))
    draw.rectangle([(340, 120), (360, 295)], fill=(255, 255, 255, 255))
    draw.polygon([(200, 180), (360, 130), (360, 175), (200, 225)], fill=(255, 255, 255, 255))
    return add_shadow(base)

def draw_appstore_icon():
    base = create_gradient_squircle((0, 175, 255), (0, 110, 245))
    draw = ImageDraw.Draw(base)
    # White letter A made of overlapping bars
    draw.line([(140, 370), (CANVAS//2, 130)], fill=(255, 255, 255, 255), width=34)
    draw.line([(CANVAS//2, 130), (372, 370)], fill=(255, 255, 255, 255), width=34)
    draw.line([(120, 300), (392, 300)], fill=(255, 255, 255, 255), width=34)
    return add_shadow(base)

def draw_settings_icon():
    base = create_gradient_squircle((185, 190, 200), (125, 130, 140))
    draw = ImageDraw.Draw(base)
    cx, cy = CANVAS // 2, CANVAS // 2
    # Silver gear teeth
    for deg in range(0, 360, 45):
        rad = math.radians(deg)
        x1 = cx + math.cos(rad) * 120
        y1 = cy + math.sin(rad) * 120
        draw.ellipse([(x1 - 32, y1 - 32), (x1 + 32, y1 + 32)], fill=(90, 95, 105, 255))
    draw.ellipse([(cx - 120, cy - 120), (cx + 120, cy + 120)], fill=(90, 95, 105, 255))
    draw.ellipse([(cx - 55, cy - 55), (cx + 55, cy + 55)], fill=(155, 160, 170, 255))
    return add_shadow(base)

def draw_terminal_icon():
    base = create_gradient_squircle((45, 45, 52), (18, 18, 22))
    draw = ImageDraw.Draw(base)
    # Green/White Prompt
    draw.line([(130, 180), (210, 240)], fill=(65, 215, 105, 255), width=24)
    draw.line([(210, 240), (130, 300)], fill=(65, 215, 105, 255), width=24)
    draw.line([(240, 300), (350, 300)], fill=(230, 230, 235, 255), width=22)
    return add_shadow(base)

def draw_notes_icon():
    base = create_gradient_squircle((255, 255, 255), (245, 245, 248))
    draw = ImageDraw.Draw(base)
    # Yellow Header Bar
    draw.rounded_rectangle([(32, 32), (CANVAS - 32, 140)], radius=30, fill=(255, 204, 0, 255))
    # Ruled lines
    for y in [210, 270, 330, 390]:
        draw.line([(80, y), (CANVAS - 80, y)], fill=(220, 220, 225, 255), width=6)
    return add_shadow(base)

def draw_calculator_icon():
    base = create_gradient_squircle((50, 50, 55), (25, 25, 28))
    draw = ImageDraw.Draw(base)
    # Grid of buttons
    colors = [(255, 149, 0), (165, 165, 165), (90, 90, 95)]
    positions = [
        ((120, 150), colors[1]), ((200, 150), colors[1]), ((280, 150), colors[1]), ((360, 150), colors[0]),
        ((120, 220), colors[2]), ((200, 220), colors[2]), ((280, 220), colors[2]), ((360, 220), colors[0]),
        ((120, 290), colors[2]), ((200, 290), colors[2]), ((280, 290), colors[2]), ((360, 290), colors[0]),
        ((160, 360), colors[2]), ((280, 360), colors[2]), ((360, 360), colors[0]),
    ]
    for (x, y), c in positions:
        r = 26
        draw.ellipse([(x - r, y - r), (x + r, y + r)], fill=c)
    return add_shadow(base)

def draw_folder_icon():
    im = Image.new("RGBA", (CANVAS, CANVAS), (0, 0, 0, 0))
    draw = ImageDraw.Draw(im)
    # Back tab
    draw.rounded_rectangle([(60, 100), (220, 220)], radius=24, fill=(0, 130, 230, 255))
    # Front pocket
    draw.rounded_rectangle([(40, 150), (CANVAS - 40, CANVAS - 70)], radius=36, fill=(0, 165, 255, 255))
    # Top highlight line
    draw.line([(50, 160), (CANVAS - 50, 160)], fill=(255, 255, 255, 120), width=4)
    return add_shadow(im)

def draw_trash_icon():
    im = Image.new("RGBA", (CANVAS, CANVAS), (0, 0, 0, 0))
    draw = ImageDraw.Draw(im)
    # Frosted glass cylinder
    draw.polygon([(120, 130), (392, 130), (350, 440), (162, 440)], fill=(220, 225, 235, 200), outline=(255, 255, 255, 160))
    # Vertical ribbing
    for x in [180, 220, 260, 300, 340]:
        draw.line([(x, 150), (x - 10, 420)], fill=(255, 255, 255, 140), width=6)
    # Rim
    draw.ellipse([(110, 110), (402, 150)], fill=(235, 240, 250, 240), outline=(255, 255, 255, 220), width=4)
    return add_shadow(im)

ICONS_BUILDERS = {
    'finder': (draw_finder_icon, ['system-file-manager', 'file-manager', 'org.xfce.thunar', 'thunar', 'org.gnome.Nautilus', 'dolphin']),
    'safari': (draw_safari_icon, ['web-browser', 'firefox-esr', 'firefox', 'browser', 'google-chrome', 'chromium']),
    'music': (draw_music_icon, ['multimedia-player', 'gnome-music', 'audio-player', 'rhythmbox', 'elisa']),
    'appstore': (draw_appstore_icon, ['softwarecenter', 'system-software-install', 'calamares', 'gnome-software']),
    'settings': (draw_settings_icon, ['preferences-system', 'xfce4-settings-manager', 'gnome-control-center', 'systemsettings']),
    'terminal': (draw_terminal_icon, ['utilities-terminal', 'terminal', 'xfce4-terminal', 'org.gnome.Terminal', 'konsole']),
    'notes': (draw_notes_icon, ['accessories-text-editor', 'mousepad', 'gedit', 'org.gnome.TextEditor', 'kwrite']),
    'calculator': (draw_calculator_icon, ['accessories-calculator', 'calc', 'gnome-calculator', 'kcalc']),
    'folder': (draw_folder_icon, ['folder', 'inode-directory', 'user-home', 'folder-documents', 'folder-download']),
    'trash': (draw_trash_icon, ['user-trash', 'user-trash-full', 'trash-empty', 'user-trash-empty'])
}

def main():
    print("==> Generating Full Vibrant macOS Sequoia/Tahoe Squircle Icon Suite...")
    for icon_name, (builder_func, aliases) in ICONS_BUILDERS.items():
        img = builder_func()
        for theme in THEMES:
            for s in SIZES:
                for cat in ['apps', 'places', 'categories']:
                    out_dir = os.path.join(TARGET_BASE, theme, f"{s}x{s}/{cat}")
                    os.makedirs(out_dir, exist_ok=True)
                    scaled = img.resize((s, s), Image.Resampling.LANCZOS)
                    scaled.save(os.path.join(out_dir, f"{icon_name}.png"))
                    for alias in aliases:
                        scaled.save(os.path.join(out_dir, f"{alias}.png"))
    print("==> All vibrant macOS icons successfully generated across all resolutions!")

if __name__ == "__main__":
    main()
