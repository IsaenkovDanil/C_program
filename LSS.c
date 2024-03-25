#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void mylist(const char *basePath) {
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    struct stat statbuf;

    if (!dir) {
        fprintf(stderr, "Failed to open directory %s\n", basePath);
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            pthread_create();
            if (lstat(path, &statbuf) != -1) {
                printf("%s", path);
                if (S_ISLNK(statbuf.st_mode)) {
                    char link_target[1024];
                    if (readlink(path, link_target, '\n') != -1) {
                        printf(" -> %s", link_target);
                    }
                } else if (statbuf.st_nlink > 1) {
                    printf(" *");
                }
                printf("\n");

                if (S_ISDIR(statbuf.st_mode)) {
                    mylist(path);
                }
            } else {
                fprintf(stderr, "Error accessing %s\n", path);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <path> [<path> ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        mylist(argv[i]);
    }

    return 0;
}
