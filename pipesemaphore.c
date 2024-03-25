#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo  *__buf;
};
int main() {
    int pipe_fd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    int sem_id;
    struct sembuf sem_op;
    int *shared_count;
    int shm_id;

    sem_id = semget(IPC_PRIVATE, 1, 0600);


    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == 0) {

        close(pipe_fd[1]);

        while (1) {

        read(pipe_fd[0], buffer, BUFFER_SIZE);

        }


    } else {
        close(pipe_fd[0]);
        int size = 0;

        while (1) {
            if (write(pipe_fd[1], buffer, BUFFER_SIZE) == -1) {
                printf("%d байт\n", size);
                break;
            }
            size += BUFFER_SIZE;
        }

        close(pipe_fd[1]);
        wait(NULL); // Ожидание завершения дочернего процесса
    }

    return 0;
}

//
// int main() {
//     int pipe_fds[2];
//     if (pipe(pipe_fds) == -1) {
//         perror("pipe");
//         exit(1);
//     }
//      int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
//     if (shmid == -1) {
//         perror("shmget");
//         exit(1);
//     }
//
//
//     int *shared_data = (int *)shmat(shmid, NULL, 0);
//     if ((int)shared_data == -1) {
//         perror("shmat");
//         exit(1);
//     }
//
//     sem_init(sem, 1, 0);
//
//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("fork");
//         exit(1);
//     }
//
//     if (pid == 0) {
//         char buffer[BUFFER_SIZE] ;
//         int count = 0;
//
//         close(pipe_fds[0]);
//
//         while (1) {
//             int written = write(pipe_fds[1], buffer, BUFFER_SIZE);
//             if (written == -1) {
//                 printf("Pipe buffer size is approximately %d bytes\n", count);
//                 break;
//             }
//             count += written;
//             sem_post(sem);
//         }
//
//         close(pipe_fds[1]);
//         exit(0);
//     } else {
//         close(pipe_fds[1]);
//
//         while (1) {
//             sem_wait(sem);
//
//             char buffer[BUFFER_SIZE];
//             if (read(pipe_fds[0], buffer, BUFFER_SIZE) == -1) {
//                 perror("read");
//                 break;
//             }
//         }
//
//         close(pipe_fds[0]);
//         wait(NULL);
//         sem_destroy(sem);
//         munmap(sem, sizeof(sem_t));
//     }
//
//     return 0;
// }
//
//  создать два процесса контролирует не заблокир ли он при переполнении Pipe
// int main() {
//     int n = 10; // Количество пассажиров
//     int m = 5;  // Вместимость трапа
//     int sem_id;
//     struct sembuf sem_op;
//  int *shared_count;
//     int shm_id;
//     // Создание семафора
//     sem_id = semget(IPC_PRIVATE, 1, 0600);
//     if (sem_id < 0) {
//         perror("semget");
//         exit(EXIT_FAILURE);
//     }
//
//     // Инициализация семафора
//     if (semctl(sem_id, 0, SETVAL, m) < 0) {
//         perror("semctl");
//         exit(EXIT_FAILURE);
//     }
//
//   // Создание разделяемой памяти
//     shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
//     if (shm_id < 0) {
//         perror("shmget");
//         exit(EXIT_FAILURE);
//     }
//
//     shared_count = (int *)shmat(shm_id, NULL, 0);
//     if (shared_count == (void *) -1) {
//         perror("shmat");
//         exit(EXIT_FAILURE);
//     }
//
//     *shared_count = 0; // Инициализация счетчика
//
//     for (int i = 0; i < n; i++) {
//         pid_t pid = fork();
//         if (pid == 0) {
//          shared_count = (int *)shmat(shm_id, NULL, 0);
//             if (shared_count == (void *) -1) {
//                 perror("shmat");
//                 exit(EXIT_FAILURE);
//             }
//
//             // Уменьшение значения семафора
//             sem_op.sem_num = 0;
//             sem_op.sem_op = -1;
//             sem_op.sem_flg = 0;
//             semop(sem_id, &sem_op, 1);
//             (*shared_count)++;
//
//             printf("Пассажир %d на трапе. Всего на трапе: %d\n", i + 1, *shared_count);
//             sleep(1); // Пассажир на трапе
//             (*shared_count)--;
//
//             // Увеличение значения семафора
//             sem_op.sem_op = 1;
//             semop(sem_id, &sem_op, 1);
//
//             printf("Пассажир %d покидает трап. Всего на трапе: %d\n", i + 1, *shared_count);
//
//             // Отсоединение от разделяемой памяти
//             if (shmdt(shared_count) < 0) {
//                 perror("shmdt");
//                 exit(EXIT_FAILURE);
//             }
//             exit(0);
//         } else if (pid < 0) {
//             perror("fork");
//             exit(EXIT_FAILURE);
//         }
//     }
//
//     // Ожидание завершения всех дочерних процессов
//     while (wait(NULL) > 0);
//   // Отсоединение и удаление разделяемой памяти в родительском процессе
//     if (shmdt(shared_count) < 0) {
//         perror("shmdt");
//         exit(EXIT_FAILURE);
//     }
//     if (shmctl(shm_id, IPC_RMID, NULL) < 0) {
//         perror("shmctl");
//         exit(EXIT_FAILURE);
//     }
//     // Удаление семафора
//     if (semctl(sem_id, 0, IPC_RMID) < 0) {
//         perror("semctl");
//         exit(EXIT_FAILURE);
//     }
//
//     return 0;
// }
// сигналы паралельные процессы
