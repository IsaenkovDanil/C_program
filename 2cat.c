#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

// Операции семафора
void p(int semid, int semnum) {
    struct sembuf sops = {semnum, -1, 0};
    semop(semid, &sops, 1);
}

void v(int semid, int semnum) {
    struct sembuf sops = {semnum, 1, 0};
    semop(semid, &sops, 1);
}

int main() {
    key_t key = ftok("/tmp", 'R');
    int shmid = shmget(key, 2 * sizeof(int), 0666 | IPC_CREAT);
    int *data = (int*)shmat(shmid, NULL, 0);

    int semid = semget(key, 2, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 1); // Устанавливаем начальные значения семафоров
    semctl(semid, 1, SETVAL, 0);

    if (fork() == 0) {
        // Reader (Consumer)
        while (1) {
            p(semid, 0); // ждем пока writer заполнит буфер
            printf("Reader read value: %d\n", data[0]);
            v(semid, 1); // сообщаем writer, что буфер свободен
            sleep(1);
        }
    } else {
        // Writer (Producer)
        int counter = 0;
        while (1) {
            p(semid, 1); // ждем пока reader освободит буфер
            data[0] = counter++;
            printf("Writer wrote value: %d\n", data[0]);
            v(semid, 0); // сообщаем reader, что буфер заполнен
            sleep(1);
        }
    }

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    semctl(semid, 1, IPC_RMID);

    return 0;
}

