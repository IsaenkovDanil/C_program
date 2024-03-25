#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#define BUFFER_SIZE 1
int count=0;
int fd[2];
void signal_handler(int sig) {
    printf("Размер буфера канала: %d байт\n", count);
    close(fd[1]); // Закрыть конец для записи
    exit(0);
}

int main() {



    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    key_t   key = ftok(".", 'S');

   int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);


    int *shared_data = (int *)shmat(shmid, NULL, 0);


 pid_t pid=fork();

     if (pid == -1) {
        perror("fork");
        return 1;
    }

    if(pid==0){
    int *shared_data = (int *)shmat(shmid, NULL, 0);
(*shared_data)=0;
    close(fd[0]);
    char buf='x';
    while(1){
     int result = write(fd[1],&buf,1);
     printf("%d %d %d\n",result,count,(*shared_data));
     if(result<=0){
          printf("!!!\n");

        kill(getppid(),SIGUSR1);
        break;
        }
        count++;
(*shared_data)++;

    }


 shmdt(shared_data);

        exit(0);
    }
    else
{
    int *shared_data = (int *)shmat(shmid, NULL, 0);
 close(fd[1]); // Закрыть конец для записи
          signal(SIGUSR1, signal_handler);
  pause(); // Ожидать сигнал

                        printf("Child: shared_data = %d\n", *shared_data);

 shmdt(shared_data);
        shmctl(shmid, IPC_RMID, NULL);

}


    return 0;
}










