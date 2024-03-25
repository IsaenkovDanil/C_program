#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
 #include <fcntl.h>
#define NUM_ITERATIONS 10
char * attach_memory_block(char*filename,int size);
bool detach_memory_block(char*block);
bool destroy_memory_block(char*filename);
#define BLOCK_SIZE 4096
#define SEM_PRODUCER_KEY 1234 // Пример ключа
#define SEM_CONSUMER_KEY 5678 // Пример ключа
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
if(argc!=1){
printf("usage - &s //no args",argv[0]);
return -1;
}
char*block =attach_memory_block(FILENAME,BLOCK_SIZE);

if(block==NULL){
    printf("ERROR : couldn't get block\n");
    return 0;
}

sem_t*sem_prod=sem_open(SEM_PRODUCER_FNAME,O_CREAT,0777,0);
if(sem_prod==SEM_FAILED){
perror("sem_open/producer");
exit(EXIT_FAILURE);
}
sem_t*sem_cons=sem_open(SEM_CONSUMER_FNAME,O_CREAT,0777,1);
if(sem_cons==SEM_FAILED){
perror("sem_open/consumer");
exit(EXIT_FAILURE);
}

while(true){
sem_wait(sem_prod);
    if(strlen(block)>0 ){
        printf("Reading: \"%s\"\n",block);
        bool done =(strcmp(block,"quit")==0);
        block[0]=0;
        if(done){break;}
}
sem_post(sem_cons);

}
sem_close(sem_cons);
sem_close(sem_prod);

detach_memory_block(block);
return 0;
}


