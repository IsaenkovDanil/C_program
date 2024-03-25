#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
char * attach_memory_block(char*filename,int size);
bool detach_memory_block(char*block);
bool destroy_memory_block(char*filename);
#define BLOCK_SIZE 4096
#define FILENAME "sharedmemory.c"
#define IPC_RESULT_ERROR (-1)
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
printf("usage - &s (no args)",argv[0]);
return -1;
}
if(destroy_memory_block(FILENAME)){
printf("Destroyed block: %s\n",FILENAME);
}else{
printf("Could not destroy blcok %s\n",FILENAME);}
return 0;
}
