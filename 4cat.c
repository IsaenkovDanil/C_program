#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

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

volatile sig_atomic_t done = 0;

void sigint_handler(int signum) {
    done = 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, sigint_handler);

    key_t key = 1234;
    int sem_id = semget(key, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    union semun init;
    init.val = 1;
    semctl(sem_id, 0, SETVAL, init);  // Reader semaphore
    init.val = 0;
    semctl(sem_id, 1, SETVAL, init);  // Writer semaphore

    int shmid = shmget(key, 1024, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    char *buffer = (char *)shmat(shmid, NULL, 0);
    if (buffer == (void *)-1) {
        perror("shmat");
        exit(1);
    }
//shmat,semctl
    pid_t pid = fork();
    if (pid == 0) {  // Reader

        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("fopen");
            exit(1);
        }
        while (!feof(file)) {
            P(sem_id, 0);
            if (fgets(buffer, 1024, file) == NULL) {
                buffer[0] = '\0';
                V(sem_id, 1);
                break;
            }
            V(sem_id, 1);
        }

        fclose(file);
        exit(0);
    } else if (pid > 0) {  // Writer
        while (!done) {
            P(sem_id, 1);
            if (strlen(buffer) == 0) {
                break;
            }
            printf("%s", buffer);
            V(sem_id, 0);
        }

        // Cleanup
        shmdt(buffer);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(sem_id, 0, IPC_RMID, NULL);
    } else {
        perror("fork");
        exit(1);
    }

    return 0;
}
