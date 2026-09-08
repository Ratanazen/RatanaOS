#include "../include/packages.h"
#include "../include/stdio.h"
#include "../include/string.h"

typedef struct {
    const char* name;
    const char* title;
    bool installed;
} package_t;

static package_t package_catalog[PACKAGE_COUNT] = {
    { "telegram", "Telegram Demo", false }
};

static package_t* find_package(const char* name) {
    if (!name) return NULL;
    for (int i = 0; i < PACKAGE_COUNT; i++) {
        if (strcmp(package_catalog[i].name, name) == 0) return &package_catalog[i];
    }
    return NULL;
}

void packages_init(void) {
    for (int i = 0; i < PACKAGE_COUNT; i++) {
        package_catalog[i].installed = false;
    }
}

void packages_list(void) {
    kprintf("\nRatanaOS built-in package catalog (volatile):\n");
    for (int i = 0; i < PACKAGE_COUNT; i++) {
        kprintf("  %-12s %-10s %s\n", package_catalog[i].name,
                package_catalog[i].installed ? "installed" : "available",
                package_catalog[i].title);
    }
    kprintf("Packages are compiled into the kernel; no network download or disk persistence exists yet.\n\n");
}

bool packages_install(const char* name) {
    package_t* package = find_package(name);
    if (!package) return false;
    package->installed = true;
    return true;
}

bool packages_remove(const char* name) {
    package_t* package = find_package(name);
    if (!package) return false;
    package->installed = false;
    return true;
}

bool packages_is_installed(const char* name) {
    package_t* package = find_package(name);
    return package && package->installed;
}

const char* packages_status(const char* name) {
    package_t* package = find_package(name);
    if (!package) return "unknown package";
    return package->installed ? "installed" : "available";
}
