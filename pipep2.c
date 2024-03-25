#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1

int get_pipe_size(int pipe_fd) {
    int size = 0;
    char buffer[BUFFER_SIZE];

    int flags = fcntl(pipe_fd, F_GETFL);
    fcntl(pipe_fd, F_SETFL, flags | O_NONBLOCK);

while (1) {
int result = write(pipe_fd, buffer, BUFFER_SIZE);
 if (result < 0) {

            if (errno == EAGAIN) {

                break;

            } else

            {
                perror("Error ");
                  return -1;

            }
        }
        size += result;
    }

    fcntl(pipe_fd, F_SETFL, flags);

    return size;
}

int main() {

    int pipe_fds[2];
    if (pipe2(pipe_fds, O_NONBLOCK) == -1) {
        perror("pipe2");
        return 1;
    }

    int pipe_size = get_pipe_size(pipe_fds[1]);
    if (pipe_size > 0)
    {

        printf("The size  buffer is: %d bytes\n", pipe_size);

    } else
    {
        printf("Failed \n");
    }

    close(pipe_fds[0]);
    close(pipe_fds[1]);

    return 0;
}
