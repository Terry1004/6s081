#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    int p[2];
    char buf[1];
    pipe(p);
    if (fork() == 0) {
        // child
        read(p[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(p[1], "0", 1);
        exit(0);
    } else {
        // parent
        write(p[1], "0", 1);
        wait((int*) 0);
        read(p[0], buf, 1);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
}