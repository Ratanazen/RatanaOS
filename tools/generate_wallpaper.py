#!/usr/bin/env python3
"""
Generates high-resolution macOS Sequoia (2026) inspired wallpapers:
1. Sequoia-Dark (Red/Purple/Pink abstract flowing waves)
2. BigSur-Day (Blue/Cyan/Sky dynamic gradient waves)
"""
import os
import math
from PIL import Image, ImageDraw, ImageFilter

OUTPUT_DIR = "ratanaos-live/config/includes.chroot/usr/share/backgrounds/ratanaos"
os.makedirs(OUTPUT_DIR, exist_ok=True)

WIDTH, HEIGHT = 3840, 2160  # 4K UHD

def generate_sequoia_dark():
    print("==> Generating 4K macOS Sequoia Dark wallpaper...")
    im = Image.new("RGBA", (WIDTH, HEIGHT), (22, 10, 28, 255))
    draw = ImageDraw.Draw(im)

    # Base gradient
    for y in range(HEIGHT):
        ratio = y / HEIGHT
        r = int(35 * (1 - ratio) + 120 * ratio * 0.7)
        g = int(12 * (1 - ratio) + 20 * ratio)
        b = int(45 * (1 - ratio) + 85 * ratio)
        draw.line([(0, y), (WIDTH, y)], fill=(r, g, b, 255))

    # Flowing glowing waves (Sequoia red/magenta curves)
    wave_overlay = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
    wdraw = ImageDraw.Draw(wave_overlay)

    # Big warm magenta/orange sweep
    points1 = []
    for x in range(0, WIDTH + 40, 20):
        y = int(HEIGHT * 0.65 + math.sin(x * 0.0012) * 450 + math.cos(x * 0.002) * 200)
        points1.append((x, y))
    points1.extend([(WIDTH, HEIGHT), (0, HEIGHT)])
    wdraw.polygon(points1, fill=(215, 45, 95, 200))

    # Deep crimson/purple wave
    points2 = []
    for x in range(0, WIDTH + 40, 20):
        y = int(HEIGHT * 0.75 + math.sin(x * 0.0015 + 1.2) * 400 + math.cos(x * 0.0008) * 250)
        points2.append((x, y))
    points2.extend([(WIDTH, HEIGHT), (0, HEIGHT)])
    wdraw.polygon(points2, fill=(130, 20, 70, 220))

    # Golden ambient highlight
    points3 = []
    for x in range(0, WIDTH + 40, 20):
        y = int(HEIGHT * 0.85 + math.sin(x * 0.002 + 2.5) * 300)
        points3.append((x, y))
    points3.extend([(WIDTH, HEIGHT), (0, HEIGHT)])
    wdraw.polygon(points3, fill=(245, 110, 60, 160))

    # Blur for smooth apple-style glass gradient
    wave_overlay = wave_overlay.filter(ImageFilter.GaussianBlur(radius=180))
    im = Image.alpha_composite(im, wave_overlay)
    
    out_path = os.path.join(OUTPUT_DIR, "ratanaos-sequoia-dark.png")
    im.convert("RGB").save(out_path, "PNG", quality=95)
    
    # Default symlink
    default_path = os.path.join(OUTPUT_DIR, "default-wallpaper.png")
    im.convert("RGB").save(default_path, "PNG", quality=95)
    print(f"Saved: {out_path}")

def generate_bigsur_day():
    print("==> Generating 4K macOS Dynamic Day wallpaper...")
    im = Image.new("RGBA", (WIDTH, HEIGHT), (25, 95, 185, 255))
    draw = ImageDraw.Draw(im)

    for y in range(HEIGHT):
        ratio = y / HEIGHT
        r = int(50 * (1 - ratio) + 15 * ratio)
        g = int(140 * (1 - ratio) + 110 * ratio)
        b = int(240 * (1 - ratio) + 210 * ratio)
        draw.line([(0, y), (WIDTH, y)], fill=(r, g, b, 255))

    wave_overlay = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
    wdraw = ImageDraw.Draw(wave_overlay)

    # Cyan/Aqua flow
    p1 = []
    for x in range(0, WIDTH + 40, 20):
        y = int(HEIGHT * 0.55 + math.sin(x * 0.001) * 350 + math.cos(x * 0.0018) * 180)
        p1.append((x, y))
    p1.extend([(WIDTH, HEIGHT), (0, HEIGHT)])
    wdraw.polygon(p1, fill=(0, 175, 240, 190))

    # Deep oceanic blue flow
    p2 = []
    for x in range(0, WIDTH + 40, 20):
        y = int(HEIGHT * 0.70 + math.sin(x * 0.0014 + 1.0) * 320)
        p2.append((x, y))
    p2.extend([(WIDTH, HEIGHT), (0, HEIGHT)])
    wdraw.polygon(p2, fill=(10, 70, 160, 210))

    wave_overlay = wave_overlay.filter(ImageFilter.GaussianBlur(radius=160))
    im = Image.alpha_composite(im, wave_overlay)

    out_path = os.path.join(OUTPUT_DIR, "ratanaos-bigsur-day.png")
    im.convert("RGB").save(out_path, "PNG", quality=95)
    print(f"Saved: {out_path}")

if __name__ == "__main__":
    generate_sequoia_dark()
    generate_bigsur_day()
