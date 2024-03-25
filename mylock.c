#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
   #include <sys/types.h>
       #include <sys/sem.h>

// void createsem(int *sid, key_t key, int members)
// {
//    int cntr;
//    union semun semopts;
//
//    printf("Attempting to create new semaphore set with %d members\n",members);
//    if((*sid = semget(key, members, IPC_CREAT|IPC_EXCL|0666)) == -1)
//    {
//       fprintf(stderr, "Semaphore set already exists!\n");
//       exit(1);
//    }
//    semopts.val = SEM_RESOURCE_MAX;
//    /* Инициализируем все элементы (может быть сделано с SETALL) */
//    for(cntr=0; cntr(get_member_count(sid)-1))
//    {
//       fprintf(stderr, "semaphore member %d out of range\n", member);
//       return;
//    }
//    /* Попытка запереть множество семафоров */
//    if(!getval(sid, member))
//    {
//       fprintf(stderr, "Semaphore resources exhausted (no lock)!\n");
//       exit(1);
//    }
//    sem_lock.sem_num = member;
//    if((semop(sid, &sem_lock, 1)) == -1)
//    {
//       fprintf(stderr, "Lock failed\n");
//       exit(1);
//    }
//    else
//       printf("Semaphore resources decremented by one (locked)\n");
//    dispval(sid, member);
// }
//
// void opensem(int *sid, key_t key)
// {
//    if((*sid = semget(key, 0, 0666)) == -1)
//    {
//       printf("Semaphore set does not exist!\n");
//       exit(1);
//    }
// }
// int getval(int sid, int member)
// {
//    int semval = semctl(sid, member, GETVAL, 0);
//    return semval;
// }
// void dispval(int sid, int member)
// {
//    int semval  = semctl(sid, member, GETVAL, 0);
//    printf("semval for member %d is %d\n", member, semval);
// }
// unsigned short get_member_count(int sid)
// {
//    union semun semopts;
//    struct semid_ds mysemds;
//    semopts.buf = &mysemds;
//    return(semopts.buf->sem_nsems);
// }
//
// void locksem(int sid, int member)
// {
//     struct sembuf sem_lock={ 0, -1, IPC_NOWAIT};
//     if( member<0 || member>(get_member_count(sid)-1))
//     {
//             fprintf(stderr, "semaphore member %d out of range\n", member);
//         return;
//         }
//
//     if(!semctl(sid, member, GETVAL, NULL))
// {
//     fprintf(stderr, "Semaphore resources ran out (no lock)!\n");
//     exit(1);
// }
// sem_lock.sem_num = member;
//     if((semop(sid, &sem_lock, 1)) == -1)
//     {
//         fprintf(stderr, "Lock failed\n");
//         exit(1);
//     }
//     else
//         printf("ресурсы переданы(locked)\n");
// dispval(sid, member);
// }
//
// void unlocksem(int sid, int member)
// {
//    struct sembuf sem_unlock={ member, 1, IPC_NOWAIT };
//    int semval;
//    if( member<0 || member>(get_member_count(sid)-1))
//    {
//       fprintf(stderr, "semaphore member %d out of range\n", member);
//       return;
//    }
//    /* Заперто ли множество семафоров? */
//    semval = getval(sid, member);
//    if(semval == SEM_RESOURCE_MAX) {
//       fprintf(stderr, "Semaphore not locked!\n");
//       exit(1);
//    }
//    sem_unlock.sem_num = member;
//    /* Попытка запереть множество семафоров */
//    if((semop(sid, &sem_unlock, 1)) == -1)
//    {
//       fprintf(stderr, "Unlock failed\n");
//       exit(1);
//    }
//    else
//       printf("Semaphore resources incremented by one (unlocked)\n");
//    dispval(sid, member);
// }



#define SEM_RESOURCE_MAX 1
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

void createsem(int *sid, key_t key)
{
union semun semopts;

if((*sid = semget(key, 1, IPC_CREAT|IPC_EXCL|0666))== -1)
{

}
semopts.val = SEM_RESOURCE_MAX;
semctl(*sid, 0, SETVAL, semopts);
}


void locksem(int sid)
{
struct sembuf sem_lock={ 0, -1, IPC_NOWAIT};

if(!semctl(sid, 1, GETVAL, 0))
{
fprintf(stderr, "Semaphore resources run out (no lock)!\n");
exit(1);
}
sem_lock.sem_num = 1;
if((semop(sid, &sem_lock, 1)) == -1)
{
fprintf(stderr, "Lock failed\n");
exit(1);
}
else
printf("Semaphore resources decremented by one (locked)\n");

}

void unlocksem(int sid)
{
   struct sembuf sem_unlock={ 0, 1, IPC_NOWAIT };
   int semval;

   if(semctl(sid, 1, GETVAL, 0) == SEM_RESOURCE_MAX) {
      fprintf(stderr, "Semaphore not locked!\n");
      exit(1);
   }
   sem_unlock.sem_num = 1;
   if((semop(sid, &sem_unlock, 1)) == -1)
   {
      fprintf(stderr, "Unlock failed\n");
      exit(1);
   }
   else
      printf("Semaphore resources incremented by one (unlocked)\n");
}












int main(int argc, char *argv[]) {
//lock неполучилось подаждать
//try lock попробовать запереть
 key_t key;
   int semset_id;
      key = ftok(argv[2], 'a');

 /*   int sem_id = semget(key, 2, IPC_CREAT | 0700);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    union semun init;
    init.val = 1;
    semctl(sem_id, 0, SETVAL, init);

*/
createsem(&semset_id, key, atoi(argv[2]));

   if (strcmp(argv[1], "lock") == 0){
        locksem(semset_id);
      }else    if (strcmp(argv[1], "ublock") == 0){

unlocksem(semset_id);
      }


    return 0;
}







