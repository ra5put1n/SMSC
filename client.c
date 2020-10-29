//client
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/sem.h>
#include<string.h>

#define KEY 0x1111

 struct data{
	int service;
	char string[100];  //service 1
	double matrix[3][3];  //service 2
	int factorial;  //service 3
	int shared_mem_id;
	int client_id;
}; 	

struct data_queue {
	struct data queue[100];
	int num;
};

struct ANS{
  int answer;
};


void my_handler(){}

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};

int main()
{
	int pid = getpid();
	signal(SIGUSR1,my_handler);
	printf(" Client program has started \n");
	// Creatring Shared Memory for queue
		key_t keyForQueue = ftok("buffer.txt",10);
		if(keyForQueue<0){
			perror("errorKeyQueueClient: ");
			exit(0);
		}
		int shmForQueue= shmget(keyForQueue,sizeof(struct data_queue),IPC_CREAT | 0666);
		if(shmForQueue<0){
			perror("errorShmClient ");
			exit(0);
		} 
		struct data_queue* q = (struct data_queue*)shmat(shmForQueue,NULL,0);
		if(q==(void*)-1){
			perror("errorQueueClient: ");
			exit(0);
		}

    int id = semget(KEY, 1, 0666 | IPC_CREAT);
    if(id < 0)
    {
        perror("semget"); exit(11);
    }
    union semun u;
    u.val = 1;
    if(semctl(id, 0, SETVAL, u) < 0)
    {
        perror("semctl"); exit(12);
    }
	// shared memory created for queue

	int c;
	
  choice: printf("Enter 1 to check whether a string is palindrome or not?\n" );
  printf("Enter 2 to find determinant of a 3x3 matrix.\n" );
  printf("Enter 3 to find factorial of an integer?\n" );
  printf("Choose the service you want to avail (1/2/3) or -1 to exit: ");
	scanf("%d",&c);

  if(semop(id, &p, 1) < 0)
  {
     perror("semop p"); exit(13);
  }
	//cs
	q->num++;
	int num = q->num;
	//cs end
  if(semop(id, &v, 1) < 0)
  {
    perror("semop p"); exit(14);
  }

  //printf("%d",c);
	//printf("q->num incremented : %d",num);
	//printf("\nChoice: %d",c);
  q->queue[num].shared_mem_id=shmForQueue;
  q->queue[num].client_id=pid;

	puts("");

  if(semop(id, &p, 1) < 0)
  {
     perror("semop p"); exit(13);
  }
	//cs
  if(c==1)
	{
		q->queue[num].service=1;
		printf("Enter string: ");
		scanf("%s",q->queue[num].string);
	}
	else if(c ==2)
	{
		q->queue[num].service=2;
		printf("Enter matrix elements: ");
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				scanf("%lf",&q->queue[num].matrix[i][j]);
			}
		}
	}
  else if(c == 3)
	{
		q->queue[num].service=3;
		printf("Enter number to find factorial: ");
		scanf("%d",&q->queue[num].factorial);
	}
	else
	{
			if(semop(id, &v, 1) < 0)
  		{
    			perror("semop p"); exit(14);
  		}
			printf("Wrong choice..\n");
			goto choice;
	}
	//cs
  if(semop(id, &v, 1) < 0)
  {
    perror("semop p"); exit(14);
  }

  puts("");
	key_t key1 = ftok("answer.txt", pid);
  if(key1==-1){
    perror("error0:");
    exit(1);
  }
  int shmid = shmget(key1, sizeof(struct ANS), IPC_CREAT | 0666);
  if(shmid<0){
    perror("error1:");
    exit(1);
  }
  struct ANS *myans=(struct ANS *)shmat(shmid,NULL,0);
  if(myans == (void *) -1){
    perror("error2:");
    exit(1);
  }

  printf("\nshmid : %d",q->queue[num].shared_mem_id);
  printf("\nclient id : %d",q->queue[num].client_id);
	pause();
	//cs
	int final_ans = myans->answer;
	//cs
  printf("\n%d",final_ans);
	
  if(c==1)
  {
    if(final_ans == 0)
      printf("\nNOT palindrome");
    else
      printf("\nPalindrome");
  }
  else
    printf("\nAnswer is: %d",final_ans);
      
  exit(0);

}