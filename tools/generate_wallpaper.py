import math
from PIL import Image, ImageDraw

width, height = 1920, 1080
image = Image.new("RGB", (width, height), (18, 20, 28))
draw = ImageDraw.Draw(image)

# Draw elegant macOS Sequoia style gradient abstract curves
for y in range(height):
    ratio = y / height
    r = int(18 + 15 * math.sin(ratio * math.pi))
    g = int(22 + 20 * math.sin(ratio * math.pi + 0.5))
    b = int(38 + 45 * math.sin(ratio * math.pi + 1.0))
    draw.line([(0, y), (width, y)], fill=(r, g, b))

# Subtle glowing aura
for radius in range(400, 50, -10):
    alpha = int(25 * (1.0 - radius / 400.0))
    glow_color = (0, int(122 * 0.7), int(255 * 0.7))
    draw.ellipse(
        [width//2 - radius*2, height//2 - radius + 100, width//2 + radius*2, height//2 + radius + 100],
        outline=glow_color, width=4
    )

image.save("ratanaos-live/config/includes.chroot/usr/share/backgrounds/ratanaos/ratanaos-default.png", "PNG")
print("Wallpaper generated at ratanaos-default.png")
