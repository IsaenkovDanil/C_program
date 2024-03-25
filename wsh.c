#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <unistd.h>

#define SEM_PRODUCER_KEY 1234 // Пример ключа
#define SEM_CONSUMER_KEY 5678 // Пример ключа
#define NUM_ITERATIONS 10
char * attach_memory_block(char*filename,int size);
bool detach_memory_block(char*block);
bool destroy_memory_block(char*filename);
#define BLOCK_SIZE 4096
#define FILENAME "sharedmemory.c"
#define IPC_RESULT_ERROR (-1)
#define SEM_PRODUCER_FNAME "/myproducer1"
#define SEM_CONSUMER_FNAME "/myconsumer1"

static int get_shared_block(char *filename,int size){
key_t key=ftok(filename,0);
if(key==IPC_RESULT_ERROR){
return IPC_RESULT_ERROR;
}
return shmget(key,size,IPC_CREAT | 0666);

}
char *attach_memory_block(char*filename,int size){
int shared_block_id=get_shared_block(filename,size);
char*result;
if(shared_block_id==IPC_RESULT_ERROR){
return 0;}
result=shmat(shared_block_id,NULL,0);
if(result==(char*)IPC_RESULT_ERROR){
return result;
}


}
bool detach_memory_block(char*block)
{
return (shmdt(block)!=IPC_RESULT_ERROR);
}
bool destroy_memory_block(char*filename)
{
int shared_block_id=get_shared_block(filename,0);
if(shared_block_id==IPC_RESULT_ERROR){
return NULL;}
return (shmctl(shared_block_id,IPC_RMID,NULL)!=IPC_RESULT_ERROR);
}


int main(int argc,char*argv[]){
if(argc!=2){
printf("usage - &s [stuff to write]",argv[0]);
return -1;
}

 int sem_prod_id = semget(SEM_PRODUCER_KEY,0 , 0666 );
    int sem_cons_id = semget(SEM_CONSUMER_KEY, 0, 0666 );

    struct sembuf p = { 0, -1, SEM_UNDO }; // операция P (ожидание)
    struct sembuf v = { 0, 1, SEM_UNDO };  // операция V (размещение)
char*block =attach_memory_block(FILENAME,BLOCK_SIZE);
if(block==NULL){
printf("ERROR : couldn't get block\n");
return -1;
}
for (int i=0;i<NUM_ITERATIONS;i++){
semop(sem_cons_id, &p, 1); // ожидание consumer
printf("Writing: \"%s\"\n",argv[1]);
strncpy(block,argv[1],BLOCK_SIZE);
semop(sem_prod_id, &v, 1); // сигнал producer
}

detach_memory_block(block);
return 0;
}






static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);
static int sem_id1;
static int sem_id2;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

//1 cons
//2 prod
int main(int argc, char *argv[]) {

    sem_id1 = semget((key_t)SEM_PRODUCER_KEY, 1, 0666 | IPC_CREAT);
    sem_id2 = semget((key_t)SEM_CONSUMER_FNAME, 1, 0666 | IPC_CREAT);

    set_semvalue(sem_id1);
    set_semvalue(sem_id2);


        if (!semaphore_p(sem_id1)) exit(EXIT_FAILURE);

        if (!semaphore_v(sem_id2)) exit(EXIT_FAILURE);


        del_semvalue(sem_id1);
        del_semvalue(sem_id2);


}
static int set_semvalue(int sem_id,int value) {
    union semun sem_union;
    sem_union.val = value;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
    return(1);
}

static void del_semvalue(int sem_id) {
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p(int sem_id) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    semop(sem_id, &sem_b, 1)

    return(1);
}

static int semaphore_v(int sem_id) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    semop(sem_id, &sem_b, 1)
    return(1);
}
