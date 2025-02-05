#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void run_cmd(int argc, char* argv[], char* extra) {
    if (fork() == 0) {
        char* new_argv[MAXARG + 1];
        for (int i = 1; i < argc; ++i)
            new_argv[i - 1] = argv[i];
        new_argv[argc - 1] = extra;
        new_argv[argc] = 0;
        exec(argv[1], new_argv);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command\n");
        exit(1);
    }

    char c;
    char buf[512];
    int bufp = 0;
    while (read(0, &c, 1) > 0) {
        if (bufp >= sizeof(buf) - 1) {
            fprintf(2, "xargs: arguments too long\n");
            exit(1);
        } else if (c == '\n') {
            buf[bufp] = '\0';
            bufp = 0;
            run_cmd(argc, argv, buf);
        } else {
            buf[bufp++] = c;
        }
    }

    if (bufp > 0) {
        buf[bufp] = '\0';
        run_cmd(argc, argv, buf);
    }

    while (wait(0) > 0);
    exit(0);
}