#include
#include
#include
#include
#include
#include
#define MAX_SEND_SIZE 80
struct mymsgbuf {
   long mtype;
   char mtext[MAX_SEND_SIZE];
};
void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text);
void read_message(int qid, struct mymsgbuf *qbuf, long type);
void remove_queue(int qid);
void change_queue_mode(int qid, char *mode);
void usage(void);
int main(int argc, char *argv[])
{
   key_t key;
   int msgqueue_id;
   struct mymsgbuf qbuf;
   if(argc == 1)
      usage();
   /* Создаем уникальный ключ через вызов ftok() */
   key = ftok(".",'m');
   /* Открываем очередь - при необходимости создаем */
   if((msgqueue_id = msgget(key, IPC_CREAT|0660)) == -1) {
      perror("msgget");
      exit(1);
   }
   switch(tolower(argv[1][0]))
   {
      case 's': send_message(msgqueue_id, (struct mymsg buf *)&qbuf,
                             atol(argv[2]), argv[3]);
                break;
      case 'r': read_message(msgqueue_id, &qbuf, atol(argv[2]));
                break;
      case 'd': remove_queue(msgqueue_id);
                break;
      case 'm': change_queue_mode(msgqueue_id, argv[2]);
                break;
      default:  usage();
   }   return(0);
}
void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text)
{
   /* Посылаем сообщение в очередь */
   printf("Sending a message ...\n");
   qbuf->mtype = type;
   strcopy(qbuf->mtext, text);
   if((msgsnd(qid, (struct msgbuf *)qbuf,
          strlen(qbuf->mtext)+1, 0)) == -1)
   {
      perror("msgsnd");
      exit(1);
   }
}
void read_message(int qid, struct mymsgbuf *qbuf, long type)
{
  /* Вычитываем сообщение из очереди */
  printf("Reading a message ...\n");
  qbuf->mtype = type;
  msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
  printf("Type: %ld Text: %s\n", qbuf->mtype, qbuf->mtext);
}
void remove_queue(int qid)
{
   /* Удаляем очередь */
   msgctl(qid, IPC_RMID, 0);
}
void change_queue_mode(int qid, char *mode)
{
   struct msqid_ds myqueue_ds;
   /* Получаем текущее состояние */
   msgctl(qid, IPC_STAT, &myqueue_ds);
   /* Меняем состояние в копии внутренней структуры данных */
   sscanf(mode, "%ho", &myqueue_ds.msg_perm.mode)
   /* Обновляем состояние в самой внутренней структуре данных */
   msgctl(qid, IPC_SET, &myqueue_ds);
}
void usage(void)
{
   fprintf(stderr, "msgtool - A utility for tinkering with msg queues\n");
   fprintf(stderr, "\nUSAGE: msgtool (s)end  \n");
   fprintf(stderr, "                 (r)ecv \n");
   fprintf(stderr, "                 (d)elete\n");
   fprintf(stderr, "                 (m)ode \n");
   exit(1);
}
