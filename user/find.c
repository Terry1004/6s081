#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char *path, char *filename) {
    int fd;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    if (st.type == T_FILE) {
        char* p = path + strlen(path);
        while (p >= path && *p != '/')
            --p;
        ++p;
        if (strcmp(p, filename) == 0) {
            fprintf(1, "%s\n", path);
        }
        close(fd);
        return;
    }

    if (st.type == T_DIR) {
        struct dirent de;
        char buf[512];
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            fprintf(2, "find: path too long\n");
            close(fd);
            return;
        }
        strcpy(buf, path);
        char* p = buf + strlen(buf);
        *(p++) = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, filename);
        }
    }
    close(fd);
}
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(2, "Usage: find path filename\n");
        exit(1);
    }
    
    find(argv[1], argv[2]);
    exit(0);
}
