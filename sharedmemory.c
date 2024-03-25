#include
#include
#include
#include
#define SEGSIZE 100
main(int argc, char *argv[])
{
   key_t key;
   int shmid, cntr;
   char *segptr;
   if(argc == 1)
      usage();
   /* Создаем особый ключ через вызов ftok() */
   key = ftok(".", 'S');
   /* Открываем разделяемый сегмкнт памяти - если нужно, создаем */
   if((shmid = shmget(key, SEGSIZE, IPC_CREAT|IPC_EXCL|0660)) == -1)
   {
      printf("Shared memory segment exists - opening as a client\n");
      /* Скорее всего, сегмент уже существует - попробуем как клиент */
      if((shmid = shmget(key, SEGSIZE, 0)) == -1)
      {
         perror("shmget");
         exit(1);
      }
   }
   else
   {
      printf("Creating new shared memory segment\n");
   }
   /* Привязывем (распределяем) разделяемый сегмент памяти в текущем
      процессе */
   if((segptr = shmat(shmid, 0, 0)) == -1)
   {
      perror("shmat");
      exit(1);
   }
   switch(tolower(argv[1][0]))
   {
      case 'w': writeshm(shmid, segptr, argv[2]);
                break;
      case 'r': readshm(shmid, segptr);
                break;
      case 'd': removeshm(shmid);
                break;
      case 'm': changemode(shmid, argv[2]);
                break;
      default: usage();
   }
}
writeshm(int shmid, char *segptr, char *text)
{
   strcpy(segptr, text);
   printf("Done...\n");
}
readshm(int shmid, char *segptr)
{
   printf("segptr: %d\n", segptr);
}
removeshm(int shmid)
{
   shmctl(shmid, IPC_RMID, 0);
   printf("Shared memory segment marked for deletion\n");
}
changemode(int shmid, char *mode)
{
   struct shmid_ds myshmds;
   /* Копируем текущее значение для внутренней структуры данных в myshmds */
   shmctl(shmid, IPC_STAT, &myshmds);
   /* Выводим на экран старые права доступа */
   printf("Old permissions were: %o\n", myshmds.shm_perm.mode);
   /* Исправляем значение режима доступа в копии */
   sscanf(mode, "%o", &myshmds.shm_perm.mode);
   /* Обновляем действующее значение режима доступа */
   shmctl(shmid, IPC_SET, &myshmds);
   printf("New permissions are: %o\n", myshmds.shm_perm.mode);
}
usage()
{
   fprintf(stderr, "shmtool - A utility for tinkering with shared memory\n");
   fprintf(stderr, "\nUSAGE: shmtool (w)rite \n");
   fprintf(stderr, "                 (r)ead\n");
   fprintf(stderr, "                 (d)elete\n");
   exit(1);   fprintf(stderr, "             (m)ode change \n");
   exit(1);
}
