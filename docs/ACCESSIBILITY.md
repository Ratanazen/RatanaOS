# ♿ RatanaOS Accessibility Support (Phase 11)

## Accessibility Verification Checklist

| # | Requirement | Test Method | Status | Notes |
|---|:---|:---|:---:|:---|
| 1 | **Orca Screen Reader** | Launch via Settings > Accessibility or \`orca\` in terminal. Verify it reads XFCE panel items, Thunar file names, and dialog buttons through the RatanaOS GTK theme. | PASSED | AT-SPI2 bridge (\`at-spi2-core\`) ensures widget labels are exposed. Custom GTK CSS does not override widget \`accessible-name\` properties. |
| 2 | **High-Contrast Theme** | Open XFCE Settings > Appearance > Style, select "HighContrast" or "Adwaita" as an alternative to RatanaOS-Dark. | PASSED | Adwaita and GNOME accessibility themes are included alongside RatanaOS themes. |
| 3 | **UI Scaling / Text Size** | Open XFCE Settings > Appearance > Fonts, adjust DPI scaling or font size. Verify panel, dock, and window titles scale proportionally. | PASSED | XFCE's Xft DPI setting and font size controls work correctly with RatanaOS theme. |
| 4 | **Keyboard-Only Navigation** | Navigate dock and top panel using Tab, Alt+Tab, and arrow keys without mouse. Verify Plank dock items are focusable and activatable via keyboard. | PASSED | Plank supports keyboard navigation. XFCE panel items are navigable via Alt+F1 (menu) and Alt+F2 (app launcher). |
| 5 | **On-Screen Keyboard** | Launch \`onboard\` from terminal or application menu. Verify it displays over other windows and accepts touch/click input. | PASSED | \`onboard\` package included and functional. |

---

## Accessibility Design Notes

1. **RatanaOS GTK Theme Compatibility**: The custom \`RatanaOS-Dark\` and
   \`RatanaOS-Light\` GTK3 themes use standard CSS properties and do not
   override \`GtkWidget::accessible-role\` or \`accessible-name\` attributes.
   Orca reads all standard XFCE widgets correctly through the theme.

2. **Traffic Light Window Buttons**: The XFWM4 traffic-light button theme
   uses standard XPM button images. XFWM4 exposes close/minimize/maximize
   actions to AT-SPI2 regardless of button visual appearance.

3. **Plank Dock**: Plank exposes dock item labels to AT-SPI2 for screen
   reader announcement. Keyboard focus via \`Super+1\` through \`Super+9\`
   shortcuts for dock item quick-launch.

4. **Magnification**: XFWM4's built-in compositor supports zoom via
   \`Alt+Scroll\` (configurable in XFCE Settings > Window Manager Tweaks >
   Accessibility).
