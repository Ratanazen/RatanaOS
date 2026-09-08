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
    kprintf("  theme <name>      - Apply theme (arch, cyber2026, matrix, ocean, amber)\n");
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
    heap_stats_t hstats = heap_get_stats();
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
    kprintf("Memory:    ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("Heap: %u KB used / %u KB total (%u blocks)\n",
            (uint32_t)(hstats.used_size / 1024), (uint32_t)(hstats.total_size / 1024), (uint32_t)hstats.num_allocations);

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("  /_/      \\_\\   ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Uptime:    ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%u seconds (%u ticks @ 100Hz)\n", sec, ticks);

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("                 ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Desktop:   ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("macOS Sequoia Aqua (VBE 1024x768x32 Framebuffer)\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("                 ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Shell:     ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("RatanaSH 64-bit v2.0 (2026)\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("                 ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Palette:   ");
    for (int i = 0; i < 8; i++) {
        vga_set_color(vga_entry_color((enum vga_color)i, (enum vga_color)i));
        kprintf("  ");
    }
    kprintf("\n                 Palette:   ");
    for (int i = 8; i < 16; i++) {
        vga_set_color(vga_entry_color((enum vga_color)i, (enum vga_color)i));
        kprintf("  ");
    }
    kprintf("\n");
    vga_set_color(old);
}

static void cmd_mem(void) {
    heap_stats_t hstats = heap_get_stats();
    size_t free_pgs = pmm_get_free_pages();
    size_t total_pgs = pmm_get_total_pages();

    kprintf("\n64-bit Memory Status:\n");
    kprintf(" -----------------------------------------------------\n");
    kprintf(" Physical Memory: %u MB Total (%u pages, 4KB/page)\n",
            (uint32_t)((total_pgs * 4) / 1024), (uint32_t)total_pgs);
    kprintf(" Free Pages:      %u (%u MB free)\n",
            (uint32_t)free_pgs, (uint32_t)((free_pgs * 4) / 1024));
    kprintf(" 64-bit Heap:     %u KB Total\n", (uint32_t)(hstats.total_size / 1024));
    kprintf(" Used Heap:       %u bytes (%u KB)\n", (uint32_t)hstats.used_size, (uint32_t)(hstats.used_size / 1024));
    kprintf(" Free Heap:       %u bytes (%u KB)\n", (uint32_t)hstats.free_size, (uint32_t)(hstats.free_size / 1024));
    kprintf(" Active Blocks:   %u\n", (uint32_t)hstats.num_allocations);
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

static void cmd_theme(char* args) {
    while (*args == ' ') args++;
    if (strcmp(args, "arch") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
        kprintf("Applied 'Arch' theme.\n");
    } else if (strcmp(args, "cyber2026") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK));
        kprintf("Applied 'Cyber 2026' theme.\n");
    } else if (strcmp(args, "matrix") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        kprintf("Applied 'Matrix' theme.\n");
    } else if (strcmp(args, "ocean") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
        vga_clear();
        kprintf("Applied 'Ocean' theme.\n");
    } else if (strcmp(args, "amber") == 0) {
        vga_set_color(vga_entry_color(VGA_COLOR_BROWN, VGA_COLOR_BLACK));
        kprintf("Applied 'Amber CRT' theme.\n");
    } else {
        kprintf("Usage: theme <arch | cyber2026 | matrix | ocean | amber>\n");
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
            kprintf("Unknown operator '%c'. Supported: +, -, *, /, %%\n", op);
            return;
    }

    kprintf("%d %c %d = %d\n", a, op, b, result);
}

static void cmd_color(char* args) {
    while (*args == ' ') args++;
    if (*args == '\0') {
        kprintf("Usage: color <fg:0-15> [bg:0-15]\n");
        return;
    }

    int fg = atoi(args);
    while (*args && *args != ' ') args++;
    while (*args == ' ') args++;

    int bg = VGA_COLOR_BLACK;
    if (*args != '\0') {
        bg = atoi(args);
    }

    if (fg < 0 || fg > 15 || bg < 0 || bg > 15) {
        kprintf("Color numbers must be between 0 and 15.\n");
        return;
    }

    vga_set_color(vga_entry_color((enum vga_color)fg, (enum vga_color)bg));
    kprintf("Color scheme updated.\n");
}

static void cmd_reboot(void) {
    kprintf("\nRebooting system...\n");
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
    __asm__ volatile ("cli; hlt");
}

static void cmd_shutdown(void) {
    kprintf("\nShutting down system...\n");

    // 1. QEMU / Bochs older ACPI power off
    outw(0xB004, 0x2000);

    // 2. QEMU modern ACPI power off
    outw(0x604, 0x2000);

    // 3. VirtualBox power off
    outw(0x4004, 0x3400);

    // 4. Cloud / ACPI DSDT power off
    outw(0x600, 0x34);

    // 5. Fallback halt
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
