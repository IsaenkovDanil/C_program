#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void P(int sem_id, int sem_num) {
    struct sembuf op = {sem_num, -1, 0};
    semop(sem_id, &op, 1);
}

void V(int sem_id, int sem_num) {
    struct sembuf op = {sem_num, 1, 0};
    semop(sem_id, &op, 1);
}

int main() {
    key_t key = 1234;
    int sem_id = semget(key, 2, IPC_CREAT | 0700);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    union semun init;
    init.val = 1;
    semctl(sem_id, 0, SETVAL, init);
    init.val = 0;
    semctl(sem_id, 1, SETVAL, init);

    int shmid = shmget(key, 1024, IPC_CREAT | 0700);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    char *buffer = (char *)shmat(shmid, NULL, 0);
    if (buffer == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) {  // Reader
        while (1) {
            P(sem_id, 0);
            int bytesRead = read(STDIN_FILENO, buffer, 1024);
            if (bytesRead < 0) {
                perror("read");
                exit(1);
            }
            V(sem_id, 1);
        }
    } else if (pid > 0) {  // Writer
        while (1) {
            P(sem_id, 1);
            int bytesWritten = write(STDOUT_FILENO, buffer, strlen(buffer));
            if (bytesWritten < 0) {
                perror("write");
                exit(1);
            }
            V(sem_id, 0);
        }
    } else {
        perror("fork");
        exit(1);
    }

    shmdt(buffer);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, NULL);

    return 0;
}

