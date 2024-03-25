#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1)
    {
        perror("pipe");
        exit(1);
    }



    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    {
        char buffer[BUFFER_SIZE];
        int count = 0;

        close(pipe_fds[0]);

        while (1)
        {

            int written = write(pipe_fds[1], buffer, BUFFER_SIZE);
            if (written == -1) {
            pid_t pid1 = fork();
             if (pid == 0)
    {
       count += written;
                printf("Pipe buffer size is  %d bytes\n", count);
    }
                printf("Pipe buffer size is  %d bytes\n", count);
                break;
            }
            count += written;
            sem_post(sem);
        }

        close(pipe_fds[1]);
        exit(0);
    }
     else
      {
        close(pipe_fds[1]);
        while (1) {
            sem_wait(sem);

            char buffer[BUFFER_SIZE];
            if (read(pipe_fds[0], buffer, BUFFER_SIZE) == -1) {
                perror("read");
                break;
            }
        }

        close(pipe_fds[0]);
        wait(NULL);
        sem_destroy(sem);
    }

    return 0;
}
