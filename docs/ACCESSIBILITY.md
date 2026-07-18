# RatanaOS Accessibility Guide

RatanaOS is committed to being usable by everyone. This document describes the accessibility features available in RatanaOS v5.0+.

## Built-in Accessibility Features

### Visual
| Feature | How to Enable |
|---|---|
| **Screen Reader** | Settings → Accessibility → Enable screen reader (Orca) |
| **High Contrast Theme** | Settings → Accessibility → High contrast theme |
| **Large Text Mode** | Settings → Accessibility → Large text mode |
| **Magnification** | Settings → Accessibility → Magnification slider (100–400%) |
| **Reduce Motion** | Settings → Accessibility → Reduce motion effects |

### Input
| Feature | How to Enable |
|---|---|
| **Sticky Keys** | Settings → Accessibility → Sticky keys |
| **Slow Keys** | Settings → Accessibility → Slow keys |
| **Mouse Keys** | Control numpad as mouse pointer |

## Screen Reader
RatanaOS bundles **Orca** as the system screen reader, which supports:
- GNOME and KDE Plasma applications
- Web browsers (Firefox, Chromium)
- Terminal output narration

To start Orca manually:
```bash
orca &
```

## Keyboard Shortcuts (Accessibility Mode)
| Shortcut | Action |
|---|---|
| `Super + Alt + A` | Toggle accessibility mode |
| `Super + =` | Zoom in |
| `Super + -` | Zoom out |
| `Super + 0` | Reset zoom |

## Reporting Accessibility Issues
If you encounter an accessibility barrier in RatanaOS, please open a GitHub issue tagged `accessibility`.
