#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string.h>

#define SEM_RESOURCE_MAX 1

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void createsem(int *sid, key_t key) {
    union semun semopts;

    // Try to create the semaphore set with 1 semaphore
    *sid = semget(key, 1, IPC_CREAT | 0666);
    if (*sid == -1) {
        perror("semget failed");
        exit(1);
    }

    // Set the semaphore's value to 1
    semopts.val = SEM_RESOURCE_MAX;
    if (semctl(*sid, 0, SETVAL, semopts) == -1) {
        perror("semctl SETVAL failed");
        exit(1);
    }
}

void locksem(int sid) {
    struct sembuf sem_lock = {0, -1, 0}; // Decrement by 1, wait if necessary

    if (semop(sid, &sem_lock, 1) == -1) {
        perror("Lock failed");
        exit(1);
    } else {
        printf("Semaphore locked\n");
    }
}

void unlocksem(int sid) {
    struct sembuf sem_unlock = {0, 1, 0}; // Increment by 1

    if (semop(sid, &sem_unlock, 1) == -1) {
        perror("Unlock failed");
        exit(1);
    } else {
        printf("Semaphore unlocked\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [lock|unlock] <key>\n", argv[0]);
        exit(1);
    }

    key_t key;
    int semset_id;

    key = ftok(argv[2], 'a');
    createsem(&semset_id, key);

    if (strcmp(argv[1], "lock") == 0) {
        locksem(semset_id);
    } else if (strcmp(argv[1], "unlock") == 0) {
        unlocksem(semset_id);
    } else {
        fprintf(stderr, "Invalid command\n");
        exit(1);
    }

    return 0;
}
