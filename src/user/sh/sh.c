#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"
#include "../../libc/include/stdlib.h"
#include "../../libc/include/string.h"
#include "../../libc/include/sys/utsname.h"

#define MAX_LINE 256
#define MAX_ARGS 16

static void print_prompt(void) {
    char cwd_buf[64];
    if (!getcwd(cwd_buf, sizeof(cwd_buf))) {
        strcpy(cwd_buf, "/");
    }
    printf("ratana@ratanaos-sh:%s$ ", cwd_buf);
}

static int parse_line(char* line, char* args[]) {
    int count = 0;
    while (*line && count < MAX_ARGS - 1) {
        while (*line == ' ' || *line == '\t' || *line == '\n') {
            *line++ = '\0';
        }
        if (*line == '\0') break;
        args[count++] = line;
        while (*line && *line != ' ' && *line != '\t' && *line != '\n') {
            line++;
        }
    }
    args[count] = NULL;
    return count;
}

static void execute_external(char* args[]) {
    int64_t pid = fork();
    if (pid == 0) {
        // Child
        char path[64];
        if (args[0][0] == '/') {
            strcpy(path, args[0]);
        } else {
            strcpy(path, "/bin/");
            strcat(path, args[0]);
        }
        execve(path, args, NULL);
        printf("sh: command not found: %s\n", args[0]);
        _exit(127);
    } else if (pid > 0) {
        int status = 0;
        waitpid(pid, &status, 0);
    } else {
        printf("sh: fork failed\n");
    }
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("\n=========================================\n");
    printf("       RatanaOS Userspace Shell (/bin/sh)\n");
    printf("=========================================\n");
    printf("Type 'help' for built-in commands or run binaries from /bin.\n\n");

    char line[MAX_LINE];
    char* args[MAX_ARGS];

    while (1) {
        print_prompt();
        int idx = 0;
        while (idx < MAX_LINE - 1) {
            int c = getchar();
            if (c == EOF || c == '\n') {
                line[idx] = '\0';
                putchar('\n');
                break;
            } else if (c == '\b' || c == 127) {
                if (idx > 0) {
                    idx--;
                    printf("\b \b");
                }
            } else if (c >= 32 && c <= 126) {
                line[idx++] = (char)c;
                putchar(c);
            }
        }
        line[idx] = '\0';

        int arg_count = parse_line(line, args);
        if (arg_count == 0) continue;

        // Built-ins
        if (strcmp(args[0], "exit") == 0) {
            printf("Exiting RatanaOS shell.\n");
            break;
        } else if (strcmp(args[0], "help") == 0) {
            printf("RatanaOS Shell Built-in Commands:\n");
            printf("  cd <dir>    - Change working directory\n");
            printf("  pwd         - Print working directory\n");
            printf("  echo <text> - Print arguments\n");
            printf("  uname       - System architecture\n");
            printf("  help        - Display this menu\n");
            printf("  exit        - Terminate shell\n");
            printf("External Executables:\n");
            printf("  /bin/hello, /bin/ls, /bin/cat, /sbin/init\n");
        } else if (strcmp(args[0], "cd") == 0) {
            const char* target = (arg_count > 1) ? args[1] : "/";
            if (chdir(target) < 0) {
                printf("cd: %s: No such directory\n", target);
            }
        } else if (strcmp(args[0], "pwd") == 0) {
            char buf[64];
            if (getcwd(buf, sizeof(buf))) {
                printf("%s\n", buf);
            }
        } else if (strcmp(args[0], "echo") == 0) {
            for (int i = 1; i < arg_count; i++) {
                printf("%s%s", args[i], (i == arg_count - 1) ? "" : " ");
            }
            putchar('\n');
        } else if (strcmp(args[0], "uname") == 0) {
            struct utsname u;
            if (uname(&u) == 0) {
                printf("%s %s %s %s %s\n", u.sysname, u.nodename, u.release, u.version, u.machine);
            }
        } else {
            execute_external(args);
        }
    }

    return 0;
}
