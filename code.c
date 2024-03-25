#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

void P(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1; // Уменьшить семафор (ожидание)
    op.sem_flg = SEM_UNDO;

    if (semop(semid, &op, 1) == -1) {
        perror("semop - P");
        exit(1);
    }
}

void V(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1; // Увеличить семафор (сигнал)
    op.sem_flg = SEM_UNDO;

    if (semop(semid, &op, 1) == -1) {
        perror("semop - V");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <printer_id> <operation>\n", argv[0]);
        return 1;
    }

    key_t key = ftok(argv[1], 1);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    int semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        return 1;
    }

    int semval = semctl(semid, 0, GETVAL);
    if (semval == -1) {
        perror("semctl - GETVAL");
        return 1;
    }

    if (strcmp(argv[2], "lock") == 0) {
        printf("Текущее значение семафора: %d\n", semval);
        if (semval == 0) {
            printf("Принтер уже заблокирован.\n");
            return 0;
        }
        P(semid);
        printf("Принтер '%s' успешно заблокирован.\n", argv[1]);
    } else if (strcmp(argv[2], "unlock") == 0) {
        V(semid);
        printf("Принтер '%s' разблокирован.\n", argv[1]);
    } else {
        fprintf(stderr, "Invalid operation. Use 'lock' or 'unlock'.\n");
        return 1;
    }

    return 0;
}
