#ifndef PACKAGES_H
#define PACKAGES_H

#include "types.h"

typedef enum {
    PACKAGE_TELEGRAM_DEMO = 0,
    PACKAGE_COUNT
} package_id_t;

void packages_init(void);
void packages_list(void);
bool packages_install(const char* name);
bool packages_remove(const char* name);
bool packages_is_installed(const char* name);
const char* packages_status(const char* name);

#endif
