#include "../include/shell.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/vga.h"
#include "../include/timer.h"
#include "../include/io.h"

static char command_buffer[SHELL_BUFFER_SIZE];
static size_t command_len = 0;

void shell_prompt(void) {
    uint8_t old_color = vga_get_color();
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("ratana");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK));
    kprintf("@");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
    kprintf("os");
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
    kprintf("\nAvailable Commands:\n");
    kprintf("  fetch             - Display system overview and ASCII logo\n");
    kprintf("  help              - Show this help menu\n");
    kprintf("  clear             - Clear the screen\n");
    kprintf("  echo <text>       - Print text to standard output\n");
    kprintf("  calc <a> <op> <b> - Simple math (+, -, *, /, %%)\n");
    kprintf("  color <fg> [bg]   - Set terminal colors (0-15)\n");
    kprintf("  uptime            - Show system uptime\n");
    kprintf("  about             - System and developer details\n");
    kprintf("  reboot            - Restart the machine\n");
    kprintf("  halt              - Halt CPU execution\n");
}

static void cmd_fetch(void) {
    uint8_t old = vga_get_color();
    uint32_t ticks = timer_get_ticks();
    uint32_t sec = ticks / 100;

    kprintf("\n");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("      /\\        ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("OS:        ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("RatanaOS x86 v1.0 (Arch-inspired)\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("     /  \\       ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Kernel:    ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("32-bit x86 Protected Mode (ELF)\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("    / /\\ \\      ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Arch:      ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("i686 / x86_64 compatible\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("   / /__\\ \\     ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Uptime:    ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("%u seconds (%u ticks)\n", sec, ticks);

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("  /_/    \\_\\    ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Shell:     ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("RatanaSH v1.0\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("                ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Display:   ");
    vga_set_color(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    kprintf("VGA Text Mode 80x25 (16 Colors)\n");

    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    kprintf("                ");
    vga_set_color(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    kprintf("Palette:   ");
    for (int i = 0; i < 8; i++) {
        vga_set_color(vga_entry_color((enum vga_color)i, (enum vga_color)i));
        kprintf("  ");
    }
    kprintf("\n                Palette:   ");
    for (int i = 8; i < 16; i++) {
        vga_set_color(vga_entry_color((enum vga_color)i, (enum vga_color)i));
        kprintf("  ");
    }
    kprintf("\n");
    vga_set_color(old);
}

static void cmd_calc(char* args) {
    while (*args == ' ') args++;
    if (*args == '\0') {
        kprintf("Usage: calc <num1> <op> <num2> (e.g. calc 42 * 2)\n");
        return;
    }

    char* p = args;
    int a = atoi(p);

    // Skip first number
    if (*p == '-' || *p == '+') p++;
    while (*p >= '0' && *p <= '9') p++;
    while (*p == ' ') p++;

    char op = *p;
    if (op == '\0') {
        kprintf("Error: Missing operator (+, -, *, /, %%)\n");
        return;
    }
    p++; // Skip operator
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
        kprintf("Colors: 0=Black, 1=Blue, 2=Green, 3=Cyan, 4=Red, 5=Magenta, 6=Brown, 7=LightGrey\n");
        kprintf("        8=DarkGrey, 9=LightBlue, 10=LightGreen, 11=LightCyan, 12=LightRed, 13=Pink, 14=Yellow, 15=White\n");
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
    kprintf("Terminal color updated (fg=%d, bg=%d).\n", fg, bg);
}

static void cmd_about(void) {
    kprintf("\nRatanaOS - High-Performance Lightweight 32-bit x86 Kernel\n");
    kprintf("Author: Ratanazen\n");
    kprintf("Features: Protected Mode GDT, IDT, PIC 8259, PIT Timer, PS/2 Keyboard, Color VGA CLI\n");
}

static void cmd_reboot(void) {
    kprintf("\nRebooting system...\n");
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE); // Pulse CPU reset line
    __asm__ volatile ("cli; hlt");
}

void shell_execute(char* command) {
    while (*command == ' ') command++;
    if (*command == '\0') return;

    if (strcmp(command, "help") == 0) {
        cmd_help();
    } else if (strcmp(command, "fetch") == 0) {
        cmd_fetch();
    } else if (strcmp(command, "clear") == 0) {
        vga_clear();
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
        cmd_about();
    } else if (strcmp(command, "reboot") == 0) {
        cmd_reboot();
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
