#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    int pleft[2];
    pipe(pleft);
    if (fork() == 0) {
        // end of pipeline
        close(pleft[1]);
        int prime;
        int readfd = pleft[0];
        while (read(readfd, &prime, 4)) {
            fprintf(1, "prime %d\n", prime);
            int pright[2];
            pipe(pright);
            if (fork() == 0) {
                // check if n is divisible by prime
                close(pright[0]);
                int n;
                while (read(readfd, &n, 4)) {
                    if (n % prime)
                        write(pright[1], &n, 4);
                }
                close(pright[1]);
                exit(0);
            }
            close(readfd);
            readfd = pright[0];
            close(pright[1]);
        }
        while (wait((int*) 0) > 0);
        exit(0);
    }
    // send primes to child
    close(pleft[0]);
    for (int i = 2; i <= 35; ++i)
        write(pleft[1], &i, 4);
    close(pleft[1]);
    wait((int*) 0);
    exit(0);
}