#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

void list_directory(const char* path) {
    struct dirent* entry;
    DIR* dir = opendir(path);

    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    const char* path = "."; // Default to the current directory

    if (argc > 1) {
        path = argv[1]; // If a path is provided as an argument, use it
    }

    list_directory(path);

    return EXIT_SUCCESS;
}

