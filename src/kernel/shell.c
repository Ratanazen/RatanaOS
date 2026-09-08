#include "../include/shell.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/vga.h"
#include "../include/timer.h"
#include "../include/io.h"
#include "../include/rtc.h"
#include "../include/heap.h"
#include "../include/pmm.h"
#include "../include/pci.h"
#include "../include/cpuid.h"
#include "../include/speaker.h"
#include "../include/matrix.h"
#include "../include/snake.h"
#include "../include/gui.h"
#include "../include/icons.h"
#include "../include/theme.h"
#include "../include/font.h"
#include "../include/dock.h"
#include "../include/settings.h"
#include "../include/packages.h"

static char command_buffer[SHELL_BUFFER_SIZE];
static size_t command_len = 0;

void shell_prompt(void) {
    uint8_t old_color = vga_get_color();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("ratana");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK));
    kprintf("@");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
    kprintf("os-x86_64");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf(" > ");
    vga_set_color(old_color);
}

void shell_init(void) {
    command_len = 0;
    command_buffer[0] = '\0';
    shell_prompt();
}

static void cmd_help(void) {
    kprintf("\nRatanaOS 64-bit (x86_64) Built-in Commands:\n");
    kprintf("  gui               - Launch Full-Feature 64-bit macOS GUI Desktop\n");
    kprintf("  fetch             - System overview & 64-bit ASCII architecture\n");
    kprintf("  date / time       - Query hardware CMOS Real-Time Clock (2026)\n");
    kprintf("  mem / free        - 64-bit Physical Memory & Dynamic Heap stats\n");
    kprintf("  pci               - Scan and enumerate all PCI bus devices\n");
    kprintf("  cpuid             - Query 64-bit CPU architecture and feature flags\n");
    kprintf("  calc <a> <op> <b> - Arithmetic calculator (+, -, *, /, %%)\n");
    kprintf("  beep [freq] [ms]  - Play tone through PC speaker (default 440Hz 200ms)\n");
    kprintf("  theme <name>      - Apply UI theme (dark, light, macos-dark, macos-light, auto, list)\n");
    kprintf("  accent <color>    - Set UI accent color (blue, purple, pink, red, orange, yellow, green, graphite)\n");
    kprintf("  scale <percent>   - Set global UI scale (80, 90, 100, 110, 125, 150, 175, 200)\n");
    kprintf("  font size <size>  - Set font size (small, regular, large, title, 8, 12, 16, 24, 32)\n");
    kprintf("  dock <size|mag>   - Configure Dock (size <40..64>, spacing <2..16>, mag <on/off>)\n");
    kprintf("  icons <theme>     - Switch macOS icon theme (whitesur, mactahoe, vector, next)\n");
    kprintf("  settings <cmd>    - Settings manager (appearance, status, save, load, reset)\n");
    kprintf("  pkg <cmd>         - Built-in packages (list, install, remove, run)\n");
    kprintf("  pacman / apt      - Aliases for pkg; supports Telegram Demo only\n");
    kprintf("  matrix            - Animated digital rain screensaver\n");
    kprintf("  snake             - Play interactive Snake arcade game\n");
    kprintf("  color <fg> [bg]   - Set terminal colors (0-15)\n");
    kprintf("  clear             - Clear terminal screen\n");
    kprintf("  echo <text>       - Print text to console\n");
    kprintf("  uptime            - Display system uptime and PIT timer ticks\n");
    kprintf("  about             - System and developer details\n");
    kprintf("  reboot            - Pulse CPU reset line to reboot\n");
    kprintf("  shutdown / poweroff - Power down Virtual Machine / ACPI hardware\n");
    kprintf("  halt              - Halt processor execution\n");
}

static void cmd_fetch(void) {
    uint8_t old = vga_get_color();
    uint32_t ticks = timer_get_ticks();
    uint32_t sec = ticks / 100;
    rtc_time_t t = rtc_get_time();
    cpu_info_t cpu = cpuid_get_info();

    kprintf("\n");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("       /\\        ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("OS:        ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("RatanaOS 64-bit (macOS Sequoia Edition)\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("      /  \\       ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Kernel:    ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("x86_64 Long Mode (4-Level Paging)\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("     / /\\ \\      ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("CPU:       ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%s\n", cpu.brand);

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("    / /__\\ \\     ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Date/Time: ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%u-%02u-%02u %02u:%02u:%02u UTC\n",
            t.year, t.month, t.day, t.hour, t.minute, t.second);

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("   / /____\\ \\    ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("UI Theme:  ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%s (Scale: %s)\n", theme_get_preset_name(theme_get_preset()), ui_scale_get_label());

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("  /_/      \\_\\   ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Icon Theme:");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf(" %s (32-bit ARGB)\n", icon_get_theme_name());

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("                 ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Uptime:    ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%u min %u sec (%u ticks)\n\n", sec / 60, sec % 60, ticks);
    vga_set_color(old);
}

static void cmd_mem(void) {
    uint64_t total_pages = pmm_get_total_pages();
    uint64_t free_pages = pmm_get_free_pages();
    uint64_t total_phys = total_pages * PAGE_SIZE;
    uint64_t free_phys = free_pages * PAGE_SIZE;
    uint64_t used_phys = (total_phys >= free_phys) ? (total_phys - free_phys) : 0;
    heap_stats_t hstats = heap_get_stats();

    kprintf("\n64-bit Memory Statistics:\n");
    kprintf("  Physical Memory Total: %u MB (%u KB)\n", (uint32_t)(total_phys / 1024 / 1024), (uint32_t)(total_phys / 1024));
    kprintf("  Physical Memory Used:  %u MB (%u KB)\n", (uint32_t)(used_phys / 1024 / 1024), (uint32_t)(used_phys / 1024));
    kprintf("  Physical Memory Free:  %u MB (%u KB)\n", (uint32_t)(free_phys / 1024 / 1024), (uint32_t)(free_phys / 1024));
    kprintf("  Dynamic Kernel Heap:   %u MB Total | %u KB Used | %u KB Free\n\n",
            (uint32_t)(hstats.total_size / 1024 / 1024),
            (uint32_t)(hstats.used_size / 1024),
            (uint32_t)(hstats.free_size / 1024));
}

static void cmd_color(char* args) {
    while (*args == ' ') args++;
    if (*args == '\0') {
        kprintf("Usage: color <foreground 0-15> [background 0-15]\n");
        return;
    }
    int fg = atoi(args);
    while (*args && *args != ' ') args++;
    while (*args == ' ') args++;
    int bg = 0;
    if (*args != '\0') {
        bg = atoi(args);
    }
    vga_set_color(vga_entry_color((enum vga_color)(fg & 0x0F), (enum vga_color)(bg & 0x0F)));
    kprintf("Color palette updated: FG=%d, BG=%d\n", fg & 0x0F, bg & 0x0F);
}

static void cmd_beep(char* args) {
    while (*args == ' ') args++;
    uint32_t freq = 440;
    uint32_t duration = 200;

    if (*args != '\0') {
        freq = (uint32_t)atoi(args);
        while (*args && *args != ' ') args++;
        while (*args == ' ') args++;
        if (*args != '\0') {
            duration = (uint32_t)atoi(args);
        }
    }

    if (freq < 20 || freq > 20000) freq = 440;
    if (duration > 5000) duration = 5000;

    kprintf("Beeping at %u Hz for %u ms...\n", freq, duration);
    speaker_beep(freq, duration);
}

static void cmd_icons(char* args) {
    while (*args == ' ') args++;
    if (strcmp(args, "whitesur") == 0) {
        icon_set_theme(ICON_THEME_WHITESUR);
        kprintf("Active macOS Icon Theme: %s\n", icon_get_theme_name());
    } else if (strcmp(args, "mactahoe") == 0) {
        icon_set_theme(ICON_THEME_MACTAHOE);
        kprintf("Active macOS Icon Theme: %s\n", icon_get_theme_name());
    } else if (strcmp(args, "vector") == 0) {
        icon_set_theme(ICON_THEME_VECTOR);
        kprintf("Active macOS Icon Theme: %s\n", icon_get_theme_name());
    } else if (strcmp(args, "next") == 0) {
        icon_theme_next();
        kprintf("Switched to Icon Theme: %s\n", icon_get_theme_name());
    } else if (*args == '\0') {
        kprintf("Current Icon Theme: %s\n", icon_get_theme_name());
        kprintf("Available Themes: whitesur, mactahoe, vector, next\n");
    } else {
        kprintf("Unknown icon theme '%s'. Usage: icons <whitesur | mactahoe | vector | next>\n", args);
    }
}

static void cmd_theme(char* args) {
    while (*args == ' ') args++;
    if (strncmp(args, "icons", 5) == 0 && (args[5] == ' ' || args[5] == '\0')) {
        cmd_icons(args + 5);
        return;
    }
    if (strcmp(args, "dark") == 0 || strcmp(args, "ratana-dark") == 0) {
        theme_set_preset(UI_THEME_RATANA_DARK);
        kprintf("\nUI Theme changed to:\n%s\n\n", theme_get_preset_name(UI_THEME_RATANA_DARK));
    } else if (strcmp(args, "light") == 0 || strcmp(args, "ratana-light") == 0) {
        theme_set_preset(UI_THEME_RATANA_LIGHT);
        kprintf("\nUI Theme changed to:\n%s\n\n", theme_get_preset_name(UI_THEME_RATANA_LIGHT));
    } else if (strcmp(args, "macos-dark") == 0) {
        theme_set_preset(UI_THEME_MACOS_DARK);
        kprintf("\nUI Theme changed to:\n%s\n\n", theme_get_preset_name(UI_THEME_MACOS_DARK));
    } else if (strcmp(args, "macos-light") == 0) {
        theme_set_preset(UI_THEME_MACOS_LIGHT);
        kprintf("\nUI Theme changed to:\n%s\n\n", theme_get_preset_name(UI_THEME_MACOS_LIGHT));
    } else if (strcmp(args, "auto") == 0) {
        theme_set_preset(UI_THEME_AUTO);
        kprintf("\nUI Theme changed to:\n%s (Dynamic CMOS RTC Scheduling)\n\n", theme_get_preset_name(UI_THEME_AUTO));
    } else if (strcmp(args, "list") == 0) {
        kprintf("\nAvailable UI Themes:\n");
        kprintf("  1. dark         (RatanaOS Dark)\n");
        kprintf("  2. light        (RatanaOS Light)\n");
        kprintf("  3. macos-dark   (macOS Dark Space Charcoal)\n");
        kprintf("  4. macos-light  (macOS Light Silver Aqua)\n");
        kprintf("  5. auto         (Automatic Day/Night)\n\n");
    } else if (strcmp(args, "arch") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
        kprintf("Applied 'Arch' terminal theme.\n");
    } else if (strcmp(args, "matrix") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        kprintf("Applied 'Matrix' terminal theme.\n");
    } else if (strcmp(args, "ocean") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
        vga_clear();
        kprintf("Applied 'Ocean' terminal theme.\n");
    } else {
        kprintf("Usage: theme <dark | light | macos-dark | macos-light | auto | list | icons [theme]>\n");
    }
}

static void cmd_accent(char* args) {
    while (*args == ' ') args++;
    if (strcmp(args, "blue") == 0) theme_set_accent(ACCENT_BLUE);
    else if (strcmp(args, "purple") == 0) theme_set_accent(ACCENT_PURPLE);
    else if (strcmp(args, "pink") == 0) theme_set_accent(ACCENT_PINK);
    else if (strcmp(args, "red") == 0) theme_set_accent(ACCENT_RED);
    else if (strcmp(args, "orange") == 0) theme_set_accent(ACCENT_ORANGE);
    else if (strcmp(args, "yellow") == 0) theme_set_accent(ACCENT_YELLOW);
    else if (strcmp(args, "green") == 0) theme_set_accent(ACCENT_GREEN);
    else if (strcmp(args, "graphite") == 0) theme_set_accent(ACCENT_GRAPHITE);
    else {
        kprintf("Current Accent: %s\n", theme_get_accent_name(theme_get_accent()));
        kprintf("Usage: accent <blue | purple | pink | red | orange | yellow | green | graphite>\n");
        return;
    }
    kprintf("UI Accent Color set to: %s\n", theme_get_accent_name(theme_get_accent()));
}

static void cmd_scale(char* args) {
    while (*args == ' ') args++;
    if (*args == '\0') {
        kprintf("UI Scale: %s\n", ui_scale_get_label());
        return;
    }
    int percent = atoi(args);
    if (percent < 50 || percent > 200) {
        kprintf("Error: Scale must be between 50%% and 200%%\n");
        return;
    }
    ui_scale_set(percent);
    dock_init();
    kprintf("\nUI Scale:\n%d%%\n\n", ui_scale_get());
}

static void cmd_font(char* args) {
    while (*args == ' ') args++;
    if (strncmp(args, "size", 4) == 0) {
        char* sz = args + 4;
        while (*sz == ' ') sz++;
        if (strcmp(sz, "small") == 0 || strcmp(sz, "12") == 0 || strcmp(sz, "8") == 0) {
            font_set_active_size(FONT_SIZE_SMALL);
        } else if (strcmp(sz, "regular") == 0 || strcmp(sz, "medium") == 0 || strcmp(sz, "16") == 0) {
            font_set_active_size(FONT_SIZE_REGULAR);
        } else if (strcmp(sz, "large") == 0 || strcmp(sz, "24") == 0) {
            font_set_active_size(FONT_SIZE_LARGE);
        } else if (strcmp(sz, "title") == 0 || strcmp(sz, "32") == 0) {
            font_set_active_size(FONT_SIZE_TITLE);
        } else {
            kprintf("Unknown font size '%s'. Options: small, regular, large, title\n", sz);
            return;
        }
        kprintf("\nFont Size:\n%s\n\n", font_get_size_name(font_get_active_size()));
    } else {
        kprintf("Usage: font size <small | regular | large | title>\n");
    }
}

static void cmd_dock(char* args) {
    while (*args == ' ') args++;
    if (strncmp(args, "size", 4) == 0) {
        char* sz = args + 4;
        while (*sz == ' ') sz++;
        int s = atoi(sz);
        if (s >= 32 && s <= 64) {
            dock_set_icon_size(s);
            kprintf("Dock Icon Size set to %d px.\n", s);
        } else {
            kprintf("Error: Dock icon size must be 32 to 64 px.\n");
        }
    } else if (strncmp(args, "spacing", 7) == 0) {
        char* sp = args + 7;
        while (*sp == ' ') sp++;
        int s = atoi(sp);
        if (s >= 2 && s <= 16) {
            dock_set_spacing(s);
            kprintf("Dock Spacing set to %d px.\n", s);
        } else {
            kprintf("Error: Dock spacing must be 2 to 16 px.\n");
        }
    } else if (strncmp(args, "mag", 3) == 0) {
        char* mg = args + 3;
        while (*mg == ' ') mg++;
        if (strcmp(mg, "on") == 0 || strcmp(mg, "1") == 0) {
            dock_set_magnification(true);
            kprintf("Dock Magnification enabled.\n");
        } else if (strcmp(mg, "off") == 0 || strcmp(mg, "0") == 0) {
            dock_set_magnification(false);
            kprintf("Dock Magnification disabled.\n");
        }
    } else {
        kprintf("Dock Status: Icon Size=%d px, Spacing=%d px, Mag=%s\n",
                dock_get_icon_size(), dock_get_spacing(), dock_get_magnification() ? "on" : "off");
        kprintf("Usage: dock <size <32-64> | spacing <2-16> | mag <on|off>>\n");
    }
}

static void cmd_settings(char* args) {
    while (*args == ' ') args++;
    if (strcmp(args, "appearance") == 0 || strcmp(args, "status") == 0 || *args == '\0') {
        settings_print();
    } else if (strcmp(args, "reset") == 0) {
        settings_reset_defaults();
        kprintf("System settings reset to factory defaults.\n");
    } else if (strcmp(args, "save") == 0) {
        settings_save();
    } else if (strcmp(args, "load") == 0) {
        settings_load();
    } else {
        kprintf("Usage: settings <appearance | status | reset | save | load>\n");
    }
}

static void cmd_package(char* args) {
    while (*args == ' ') args++;
    if (strcmp(args, "list") == 0 || *args == '\0') {
        packages_list();
    } else if (strncmp(args, "install ", 8) == 0) {
        const char* name = args + 8;
        if (packages_install(name)) kprintf("Installed %s for this boot session.\n", name);
        else kprintf("Package '%s' is not in the built-in catalog.\n", name);
    } else if (strncmp(args, "remove ", 7) == 0) {
        const char* name = args + 7;
        if (packages_remove(name)) kprintf("Removed %s from this boot session.\n", name);
        else kprintf("Package '%s' is not in the built-in catalog.\n", name);
    } else if (strncmp(args, "run ", 4) == 0) {
        const char* name = args + 4;
        if (!packages_is_installed(name)) {
            kprintf("Install '%s' first: pkg install %s\n", name, name);
        } else if (gui_launch_app(name)) {
            gui_start();
        } else {
            kprintf("Package '%s' has no GUI launcher.\n", name);
        }
    } else {
        kprintf("Usage: pkg <list | install telegram | remove telegram | run telegram>\n");
    }
}

static void cmd_package_alias(char* manager, char* args) {
    while (*args == ' ') args++;
    if (strcmp(manager, "pacman") == 0 && strncmp(args, "-S ", 3) == 0) {
        char command[64] = "install ";
        strncat(command, args + 3, sizeof(command) - strlen(command) - 1);
        cmd_package(command);
    } else if (strcmp(manager, "apt") == 0 && strncmp(args, "install ", 8) == 0) {
        cmd_package(args);
    } else if (strcmp(args, "list") == 0) {
        cmd_package(args);
    } else {
        kprintf("%s is a RatanaOS pkg alias. Try '%s list' or '%s install telegram'.\n", manager, manager, manager);
    }
}

static void cmd_calc(char* args) {
    while (*args == ' ') args++;
    if (*args == '\0') {
        kprintf("Usage: calc <num1> <op> <num2> (e.g. calc 42 * 2)\n");
        return;
    }

    char* p = args;
    int a = atoi(p);

    if (*p == '-' || *p == '+') p++;
    while (*p >= '0' && *p <= '9') p++;
    while (*p == ' ') p++;

    char op = *p;
    if (op == '\0') {
        kprintf("Error: Missing operator (+, -, *, /, %%)\n");
        return;
    }
    p++;
    while (*p == ' ') p++;

    if (*p == '\0') {
        kprintf("Error: Missing second operand\n");
        return;
    }
    int b = atoi(p);

    int result = 0;
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0) {
                kprintf("Error: Division by zero!\n");
                return;
            }
            result = a / b;
            break;
        case '%':
            if (b == 0) {
                kprintf("Error: Division by zero!\n");
                return;
            }
            result = a % b;
            break;
        default:
            kprintf("Unknown operator: %c\n", op);
            return;
    }

    kprintf("%d %c %d = %d\n", a, op, b, result);
}

static void cmd_reboot(void) {
    kprintf("Rebooting system...\n");
    timer_sleep_ms(500);
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
    __asm__ volatile ("hlt");
}

static void cmd_shutdown(void) {
    kprintf("Powering off RatanaOS system...\n");
    timer_sleep_ms(300);

    outw(0xB004, 0x2000);
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
    outw(0x600, 0x34);

    kprintf("System halted. You may safely turn off your machine.\n");
    __asm__ volatile ("cli; hlt");
}

void shell_execute(char* command) {
    while (*command == ' ') command++;
    if (*command == '\0') return;

    if (strcmp(command, "gui") == 0 || strcmp(command, "startx") == 0 || strcmp(command, "desktop") == 0) {
        kprintf("Starting RatanaOS 64-bit macOS Desktop Environment...\n");
        gui_start();
        kprintf("\nReturned to RatanaOS CLI.\n");
    } else if (strcmp(command, "help") == 0) {
        cmd_help();
    } else if (strcmp(command, "fetch") == 0) {
        cmd_fetch();
    } else if (strcmp(command, "clear") == 0) {
        vga_clear();
    } else if (strcmp(command, "date") == 0 || strcmp(command, "time") == 0) {
        rtc_print_formatted();
    } else if (strcmp(command, "mem") == 0 || strcmp(command, "free") == 0) {
        cmd_mem();
    } else if (strcmp(command, "pci") == 0) {
        pci_scan_all_buses();
    } else if (strcmp(command, "cpuid") == 0) {
        cpuid_print_info();
    } else if (strncmp(command, "beep", 4) == 0 && (command[4] == ' ' || command[4] == '\0')) {
        cmd_beep(command + 4);
    } else if (strncmp(command, "theme", 5) == 0 && (command[5] == ' ' || command[5] == '\0')) {
        cmd_theme(command + 5);
    } else if (strncmp(command, "accent", 6) == 0 && (command[6] == ' ' || command[6] == '\0')) {
        cmd_accent(command + 6);
    } else if (strncmp(command, "scale", 5) == 0 && (command[5] == ' ' || command[5] == '\0')) {
        cmd_scale(command + 5);
    } else if (strncmp(command, "font", 4) == 0 && (command[4] == ' ' || command[4] == '\0')) {
        cmd_font(command + 4);
    } else if (strncmp(command, "dock", 4) == 0 && (command[4] == ' ' || command[4] == '\0')) {
        cmd_dock(command + 4);
    } else if (strncmp(command, "icons", 5) == 0 && (command[5] == ' ' || command[5] == '\0')) {
        cmd_icons(command + 5);
    } else if (strncmp(command, "settings", 8) == 0 && (command[8] == ' ' || command[8] == '\0')) {
        cmd_settings(command + 8);
    } else if (strncmp(command, "pkg", 3) == 0 && (command[3] == ' ' || command[3] == '\0')) {
        cmd_package(command + 3);
    } else if (strncmp(command, "pacman", 6) == 0 && (command[6] == ' ' || command[6] == '\0')) {
        cmd_package_alias("pacman", command + 6);
    } else if (strncmp(command, "apt", 3) == 0 && (command[3] == ' ' || command[3] == '\0')) {
        cmd_package_alias("apt", command + 3);
    } else if (strcmp(command, "matrix") == 0) {
        matrix_run();
    } else if (strcmp(command, "snake") == 0) {
        snake_game_start();
    } else if (strncmp(command, "echo", 4) == 0 && (command[4] == ' ' || command[4] == '\0')) {
        char* text = command + 4;
        while (*text == ' ') text++;
        kprintf("%s\n", text);
    } else if (strncmp(command, "calc", 4) == 0 && (command[4] == ' ' || command[4] == '\0')) {
        cmd_calc(command + 4);
    } else if (strncmp(command, "color", 5) == 0 && (command[5] == ' ' || command[5] == '\0')) {
        cmd_color(command + 5);
    } else if (strcmp(command, "uptime") == 0) {
        uint32_t ticks = timer_get_ticks();
        kprintf("Uptime: %u seconds (%u ticks @ 100Hz)\n", ticks / 100, ticks);
    } else if (strcmp(command, "about") == 0) {
        kprintf("\nRatanaOS 64-bit (x86_64 Long Mode) Operating System\n");
        kprintf("Author: Ratanazen\n");
        kprintf("Architecture: 64-bit Long Mode, 4-Level Paging, GDT64, IDT64, macOS Sequoia GUI Desktop\n");
    } else if (strcmp(command, "reboot") == 0) {
        cmd_reboot();
    } else if (strcmp(command, "shutdown") == 0 || strcmp(command, "poweroff") == 0) {
        cmd_shutdown();
    } else if (strcmp(command, "halt") == 0) {
        kprintf("Halting system.\n");
        __asm__ volatile ("cli; hlt");
    } else {
        kprintf("Unknown command: \"%s\". Type 'help' for available commands.\n", command);
    }
}

void shell_update(char key) {
    if (key == '\n') {
        putchar('\n');
        command_buffer[command_len] = '\0';
        shell_execute(command_buffer);
        command_len = 0;
        command_buffer[0] = '\0';
        shell_prompt();
    } else if (key == '\b') {
        if (command_len > 0) {
            command_len--;
            command_buffer[command_len] = '\0';
            putchar('\b');
        }
    } else if (key >= ' ' && key <= '~') {
        if (command_len < SHELL_BUFFER_SIZE - 1) {
            command_buffer[command_len++] = key;
            command_buffer[command_len] = '\0';
            putchar(key);
        }
    }
}
