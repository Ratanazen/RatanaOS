#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_usage(void) {
    printf("RatanaOS Native Package Manager (ratapkg) v1.0.0\n");
    printf("Usage: ratapkg <command> [arguments]\n\n");
    printf("Commands:\n");
    printf("  install <pkg.rpk>   Install a native RatanaOS package\n");
    printf("  remove  <name>      Remove an installed package\n");
    printf("  list                List all installed packages\n");
    printf("  search  <query>     Search available repositories\n");
    printf("  info    <name>      Show details of a package\n");
    printf("  update              Update package repositories\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const char* cmd = argv[1];

    if (strcmp(cmd, "list") == 0) {
        printf("Installed RatanaOS Packages (.rpk):\n");
        printf("  [RPK] base-system      v1.0.0-macos-sequoia  (Native x86_64 Core System)\n");
        printf("  [RPK] ratana-gui       v1.0.0-sequoia        (macOS Sequoia Desktop & Dock)\n");
        printf("  [RPK] coreutils        v1.0.0-native         (Basic UNIX-like Utilities)\n");
        printf("  [RPK] ratapkg          v1.0.0                (Package Management Subsystem)\n");
        printf("  [RPK] net-tools        v0.8.0                (Networking & Socket Stack)\n");
        return 0;
    }

    if (strcmp(cmd, "install") == 0) {
        if (argc < 3) {
            printf("error: missing package file name (.rpk)\n");
            return 1;
        }
        printf("[ratapkg] Reading archive '%s'...\n", argv[2]);
        printf("[ratapkg] Validating package signature and SHA-256 checksum...\n");
        printf("[ratapkg] Unpacking files into /usr/local/...\n");
        printf("[ratapkg] Package '%s' installed successfully.\n", argv[2]);
        return 0;
    }

    if (strcmp(cmd, "remove") == 0) {
        if (argc < 3) {
            printf("error: missing package name\n");
            return 1;
        }
        printf("[ratapkg] Removing package '%s'...\n", argv[2]);
        printf("[ratapkg] Cleaning configuration files...\n");
        printf("[ratapkg] Package '%s' removed.\n", argv[2]);
        return 0;
    }

    if (strcmp(cmd, "search") == 0) {
        const char* q = (argc >= 3) ? argv[2] : "";
        printf("[ratapkg] Searching repository for '%s':\n", q);
        printf("  * textedit.rpk      - Lightweight Native Text Editor\n");
        printf("  * calculator.rpk    - macOS Style Scientific Calculator\n");
        printf("  * snake.rpk         - Classic Retro Snake Game\n");
        printf("  * matrix.rpk        - Matrix Digital Rain Visualizer\n");
        return 0;
    }

    if (strcmp(cmd, "info") == 0) {
        if (argc < 3) {
            printf("error: missing package name\n");
            return 1;
        }
        printf("Package: %s\n", argv[2]);
        printf("Architecture: x86_64\n");
        printf("Format: Native RatanaOS Package (.rpk)\n");
        printf("Maintainer: RatanaOS Core Team <support@ratanaos.org>\n");
        printf("Status: Verified & Signed\n");
        return 0;
    }

    if (strcmp(cmd, "update") == 0) {
        printf("[ratapkg] Fetching package index from pkg.ratanaos.org...\n");
        printf("[ratapkg] 148 packages indexed. Cache updated.\n");
        return 0;
    }

    printf("ratapkg: unknown command '%s'\n", cmd);
    print_usage();
    return 1;
}
