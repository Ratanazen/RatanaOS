#ifndef MENUBAR_H
#define MENUBAR_H

#include "types.h"

#define MENUBAR_HEIGHT 24

typedef enum {
    MENUBAR_HIT_NONE = 0,
    MENUBAR_HIT_APPLE_LOGO,
    MENUBAR_HIT_ITEM_ABOUT,
    MENUBAR_HIT_ITEM_SETTINGS,
    MENUBAR_HIT_ITEM_TERMINAL,
    MENUBAR_HIT_ITEM_EXIT_CLI,
    MENUBAR_HIT_MENU_FILE,
    MENUBAR_HIT_MENU_EDIT,
    MENUBAR_HIT_MENU_VIEW,
    MENUBAR_HIT_MENU_WINDOW,
    MENUBAR_HIT_MENU_HELP
} menubar_hit_t;

void menubar_init(void);
void menubar_draw(const char* active_app_name);
menubar_hit_t menubar_hit_test(int mx, int my);
bool menubar_is_apple_open(void);
void menubar_toggle_apple(void);
void menubar_close_apple(void);

#endif // MENUBAR_H
