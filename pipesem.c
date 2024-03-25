#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>

#define BUFFER_SIZE 1

int main() {
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        exit(1);
    }

    // Создаем семафор в разделяемой памяти
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(sem, 1, 0);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // Дочерний процесс (писатель)
        char buffer[BUFFER_SIZE] = {'x'};
        int count = 0;

        close(pipe_fds[0]); // Закрываем неиспользуемый конец канала

        while (1) {
            int written = write(pipe_fds[1], buffer, BUFFER_SIZE);
            if (written == -1) {
                printf("Pipe buffer size is approximately %d bytes\n", count);
                break;
            }
            count += written;
            sem_post(sem); // Уведомляем читателя
        }

        close(pipe_fds[1]);
        exit(0);
    } else { // Родительский процесс (читатель)
        close(pipe_fds[1]); // Закрываем неиспользуемый конец канала

        while (1) {
            sem_wait(sem); // Ожидаем уведомления от писателя

            char buffer[BUFFER_SIZE];
            if (read(pipe_fds[0], buffer, BUFFER_SIZE) == -1) {
                perror("read");
                break;
            }
        }

        close(pipe_fds[0]);
        wait(NULL); // Ждем завершения дочернего процесса
        sem_destroy(sem);
        munmap(sem, sizeof(sem_t));
    }

    return 0;
}
