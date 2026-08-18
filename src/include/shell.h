#ifndef SHELL_H
#define SHELL_H

#include "types.h"

#define SHELL_BUFFER_SIZE 256

void shell_init(void);
void shell_update(char key);
void shell_execute(char* command);
void shell_prompt(void);

#endif // SHELL_H
