#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
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

sem_t*sem_prod=sem_open(SEM_PRODUCER_FNAME,0);
if(sem_prod==SEM_FAILED){
perror("sem_open/producer");
exit(EXIT_FAILURE);
}
sem_t*sem_cons=sem_open(SEM_CONSUMER_FNAME,0);
if(sem_cons==SEM_FAILED){
perror("sem_open/consumer");
exit(EXIT_FAILURE);
}
char*block =attach_memory_block(FILENAME,BLOCK_SIZE);
if(block==NULL){
printf("ERROR : couldn't get block\n");
return -1;
}
for (int i=0;i<NUM_ITERATIONS;i++){
    sem_wait(sem_cons);
    printf("Writing: \"%s\"\n",argv[1]);
    strncpy(block,argv[1],BLOCK_SIZE);
    sem_post(sem_prod);
}
sem_close(sem_cons);
sem_close(sem_prod);
detach_memory_block(block);
return 0;
}

