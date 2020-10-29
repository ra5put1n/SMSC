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
	int err;
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
		//Creatring Shared Memory for queue
		key_t keyForQueue = ftok("request.txt",10);
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

    //Creating shared memory for service to client 
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
	
    //System semaphore implementation
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

	//taking input of service choice and respective data
	int c;
  choice: printf("\nEnter 1 to check whether a string is palindrome or not?\n" );
  printf("Enter 2 to find determinant of a 3x3 matrix.\n" );
  printf("Enter 3 to find factorial of an integer?\n" );
  printf("Choose the service you want to avail (1/2/3) or -1 to exit: ");
	scanf("%d",&c);
	puts("");

	int temp_fac;
	double temp_mat[3][3];
	char temp_string[100];
  
  if(c==1)
	{
		printf("Enter string: ");
		scanf("%s",temp_string);
	}
	else if(c ==2)
	{
		printf("Enter matrix elements: \n");
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				scanf("%lf",&temp_mat[i][j]);
			}
		}
	}
  else if(c == 3)
	{
		printf("Enter number to find factorial: ");
		scanf("%d",&temp_fac);
	}
	else if (c==-1)
	{
		printf("Exiting...\n");
		shmctl(shmid, IPC_RMID, NULL);
		exit(0);
	}
	else
	{
			printf("Wrong choice..\n");
			goto choice;
	}
	
	//cs start
	if(semop(id, &p, 1) < 0){
     perror("semop p"); exit(13);
  }

	q->num++;
	int num = q->num;
	q->queue[num].shared_mem_id=shmid;
  q->queue[num].client_id=pid;

	if(c == 1)
	{
		q->queue[num].service = 1;
		strcpy(q->queue[num].string,temp_string);
	}
	else if(c == 2)
	{	
		q->queue[num].service = 2;
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				q->queue[num].matrix[i][j] = temp_mat[i][j];
			}
		}
	}
	else if(c == 3)
	{
		q->queue[num].service = 3;
		q->queue[num].factorial = temp_fac;
	}
  if(semop(id, &v, 1) < 0)
  {
    perror("semop p"); exit(14);
  }
	//cs end
  puts("");

	pause();
	double final_ans = myans->answer;
	int err = myans->err;
	if(err)
	{
		printf("Error in inputs.\n");
	}
	else
	{
			if(c==1)
  		{
				if(final_ans == 0)
					printf("NOT a palindrome\n");
				else
					printf("Palindrome\n");
			}
			else
				printf("Answer is: %.2lf\n",final_ans);
	} 
	//clearing shared memory
	shmctl(shmid, IPC_RMID, NULL);
  exit(0);

}
