//server
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<sys/sem.h>

#define KEY 0x1111

 struct data{
	int service;
	char string[100];  //service 1
	double matrix[3][3];  //service 2
	int factorial;  //service 3
	int shared_mem_id;
	int client_id;
}input_data; 	

int glob_shm;

struct data_queue {
	struct data queue[100];
	int num;
};

void my_handler(){}

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};

void handle_sigint()
{ 
  char shar_id[100];
  snprintf(shar_id,100,"%d",glob_shm);
  char command[100];
	strcpy(command,"ipcrm -m ");
  strcat(command,shar_id);
	system(command);
  printf("\nRequest Queue is cleared.\nServer is Exiting.\n"); 
  exit(0);
} 

int main()
{
  char mat[3][3][100];
  char fact[100];
  char shared_id[100];
  char clint_id[100];
	signal(SIGUSR1,my_handler);
	printf("---- Server program has started ----- \n");

	//Creating Shared Memory for queue
		key_t key = ftok("request.txt",10);
		if(key<0){
			perror("errorKeyQueueServer: ");
			exit(0);
		}
		int shmid= shmget(key,sizeof(struct data_queue),IPC_CREAT | 0666);
		glob_shm = shmid;
		if(shmid<0){
			perror("errorShmServer: ");
			exit(0);
		} 
		struct data_queue* q = (struct data_queue*)shmat(shmid,NULL,0);
		if(q==(void*)-1){
			perror("errorQueueServer: ");
			exit(0);
		}

		//semaphore implementation
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

	//shared memory variables initialize
	q->num = -1;
	int front,back=0;
	printf("\nReady for Operations!\n");

while(1)
	{
     signal(SIGINT, handle_sigint); 
			//cs start
      if(semop(id, &p, 1) < 0){
        perror("semop p"); exit(13);
      }
			
			front = q->num;

      if(semop(id, &v, 1) < 0){
        perror("semop p"); exit(14);
      }
			//cs end

			if(front == -1){
				puts("Now Serving Clients..");
			}
			else if(front >= back)
			{
				//cs start
        if(semop(id, &p, 1) < 0){
          perror("semop p"); exit(13);
        }
				
				input_data = q->queue[back];
				
        if(semop(id, &v, 1) < 0){
          perror("semop p"); exit(14);
        }
				//cs end

				int pid,ch;
				ch = input_data.service;				
				pid = fork();

				if(pid == 0)
				{
					//snprintf changes from int to str
          snprintf(shared_id,100,"%d",input_data.shared_mem_id);
          snprintf(clint_id,100,"%d",input_data.client_id);
					if(ch == 1)
					{
						execl("./service1", "./service1", input_data.string,shared_id,clint_id, NULL);
						exit(0);
					}
					else if(ch == 2)
					{
            for(int i=0;i<3;i++){
              for(int j=0;j<3;j++){
                snprintf(mat[i][j],100,"%lf",input_data.matrix[i][j]);
              }
            }
						execl("./service2", "./service2", mat[0][0],mat[0][1],mat[0][2],mat[1][0],mat[1][1],mat[1][2],mat[2][0],mat[2][1],mat[2][2],shared_id,clint_id, NULL);
						exit(0);
					}
					else if(ch == 3)
					{
            snprintf(fact,100,"%d",input_data.factorial);
						execl("./service3", "./service3",fact,shared_id,clint_id, NULL);
						exit(0);
					}
      	}
				back++;
			}
			else 
			{
			  back= 0;
				//cs
        if(semop(id, &p, 1) < 0)
        {
          perror("semop p"); exit(13);
        }
				q->num = -1;
        if(semop(id, &v, 1) < 0)
        {
          perror("semop p"); exit(14);
        }
				//cs end
			}
			sleep(1);
  }	
}