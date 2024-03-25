#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, " %s \n", argv[0]);
        exit(EXIT_FAILURE);
    }
  struct stat lstat1, lstat2;
    struct stat stat1, stat2;
stat(argv[1], &stat1) ;
 stat(argv[2], &stat2);
lstat(argv[1], &lstat1) ;
lstat(argv[2], &lstat2);
if (lstat1.st_ino == lstat2.st_ino) {
        printf(" hardlink друг друга\n");
    }else
    {
     switch(lstat1.st_mode & S_IFMT){
    case S_IFLNK:
        printf("link\n");
        break;
    case S_IFREG:
        printf("not a link\n");
        break;
     }
        switch(lstat2.st_mode & S_IFMT){
    case S_IFLNK:
        printf("link\n");
        break;
    case S_IFREG:
        printf("not a link\n");
        break;
        }
     if (S_ISLNK(lstat1.st_ino) || S_ISLNK(lstat2.st_ino)) {
        printf("Один symlink\n");
    } else {
        printf(" разные\n");
    }
//если равны то


 /*S_ISLNK st_ino
 switch(stbuf.st_mode & S_IFMT){
    case S_IFLNK:
        printf("link\n");
        break;
    case S_IFREG:
        printf("not a link\n");
        break;
}*/



    return 0;
}
