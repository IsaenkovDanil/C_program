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


    if (!dir) {
        fprintf(stderr, "Failed %s\n", basePath);
        return;
    }
/*
if (dir == NULL) {
perror("opendir failed");
exit(EXIT_FAILURE);
}
*/
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            printf("%s\n", path);

            struct stat statbuf;
            if (stat(path, &statbuf) != -1) {
                if (S_ISDIR(statbuf.st_mode)) {
                    mylist(path);
                }
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    struct stat statbuf;


    for (int i = 1; i < argc; i++) {
        if (stat(argv[i], &statbuf) != -1) {
            if (S_ISDIR(statbuf.st_mode)) {
                mylist(argv[i]);
            } else if (S_ISREG(statbuf.st_mode)) {
                printf("%s\n", argv[i]);
            }
        } else {
            fprintf(stderr, "Error '%s'.\n", argv[i]);
        }
    }

    return 0;
}
