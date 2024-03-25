#include <stdio.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

void func() {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    printf("\e[H\e[J");
    printf("\e[%d;%dH", size.ws_row / 2, size.ws_col / 2);
    printf("size : %d x %d",size.ws_row,size.ws_col);
    fflush(stdout);
}
void goodbye()
{ struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    printf("\e[%d;%dH", size.ws_row , 0);
        printf("goodbye \n");
exit(0);

}
int main(int argc, char *argv[]) {
    signal(SIGWINCH, func);
    signal(SIGINT, goodbye);

    while (1) {
        sleep(1);
    }

    return 0;
}
