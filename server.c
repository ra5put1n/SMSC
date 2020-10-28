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

struct data_queue {
	struct data queue[100];
	int num;
};

void my_handler(){}

// IN CASE IF snprintf() DOES NOT WORK..
// void tostring(char str[], int num)
// {
//     int rem, len = 0, n;
//     n=num;
//     while (n != 0){
//         len++;
//         n /= 10;
//     }
//     for (int i = 0; i < len; i++){
//         rem = num % 10;
//         num = num / 10;
//         str[len - (i + 1)] = rem + '0';
//     }
//     str[len] = '\0';
// }

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};

int main()
{
  char mat[3][3][100];
  char fact[100];
	signal(SIGUSR1,my_handler);
	printf(" Server program has started \n");
	// Creatring Shared Memory for queue
		key_t key = ftok("buffer.txt",10);
		if(key<0){
			perror("errorKeyQueueServer: ");
			exit(0);
		}
		int shmid= shmget(key,sizeof(struct data_queue),IPC_CREAT | 0666);
		if(shmid<0){
			perror("errorShmServer: ");
			exit(0);
		} 
		struct data_queue* q = (struct data_queue*)shmat(shmid,NULL,0);
		if(q==(void*)-1){
			perror("errorQueueServer: ");
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
	q->num = -1;
	int front,back=0;
	printf("\nReady for Operations!\n");

while(1)
	{
      if(semop(id, &p, 1) < 0){
        perror("semop p"); exit(13);
      }
			//cs
			front = q->num;
			//cs end
      if(semop(id, &v, 1) < 0){
        perror("semop p"); exit(14);
      }

      //printf("front : %d,  back : %d ",front,back);
			if(front == -1){
				puts("Waiting for clients..");
			}
			else if(front >= back)
			{
        if(semop(id, &p, 1) < 0){
          perror("semop p"); exit(13);
        }
				//cs
				input_data = q->queue[back];
				//cs end
        if(semop(id, &v, 1) < 0){
          perror("semop p"); exit(14);
        }

				int pid,ch;
				ch = input_data.service;
				
				pid = fork();
        //printf("pid : %d",pid);
				if(pid == 0)
				{
					printf("\nChoice of service is: %d",ch);
					printf("\npid of child: %d",getpid());
					if(ch == 1)
					{
						printf("\nstrings :\n");
						puts(input_data.string);
						//execl("./service1", "./service1", input_data.string,input_data.shared_mem_id,input_data.client_id, NULL);
						exit(0);
					}
					else if(ch == 2)
					{
						printf("\nMatrix stuff\n");
            for(int i=0;i<3;i++){
              for(int j=0;j<3;j++){
                //printf("\n%lf",input_data.matrix[i][j]);
                snprintf(mat[i][j],100,"%lf",input_data.matrix[i][j]);
                puts(mat[i][j]);
              }
            }
						//execl("./service2", "./service2", input_data.matrix[0][0],input_data.matrix[0][1],input_data.matrix[0][2],input_data.matrix[1][0],input_data.matrix[1][1],input_data.matrix[1][2],input_data.matrix[2][0],input_data.matrix[2][1],input_data.matrix[2][2],input_data.shared_mem_id,input_data.client_id, NULL);
						exit(0);
					}
					else if(ch == 3)
					{
						puts("\nIn factorial");
						printf("\nfactorial:%d", input_data.factorial);
            snprintf(fact,100,"%d",input_data.factorial);
            //tostring(fact, input_data.factorial);
            puts(fact);
						//execl("./service3", "./service3", input_data.factorial,input_data.shared_mem_id,input_data.client_id, NULL);
						exit(0);
					}
					else
					{
						puts("\nNo choice but to die");
						exit(0);
					}
      	}
				back++;
			}
			else 
			{
				//puts("\nresetting front and back");
			  back= 0;
        if(semop(id, &p, 1) < 0)
        {
          perror("semop p"); exit(13);
        }
				//cs
				q->num = -1;
				//cs end
        if(semop(id, &v, 1) < 0)
        {
          perror("semop p"); exit(14);
        }
			}
			sleep(1);
  }	
	printf("exiting sucessfully");
}