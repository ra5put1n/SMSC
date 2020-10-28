#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
 struct data{
	int service;
	char string[20];  //service 1
	char * matrix[3][3];  //service 2
	char * factorial;  //service 3
	int shared_mem_id;
	int client_id;
}input_data; 	

struct data_queue {
	struct data queue[100];
	int num;
};

void my_handler(){}

int main()
{
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
	// shared memory created for queue
	q->num = -1;
	int front,back=0;
	printf("\nReady for Operations!\n");

while(1)
	{
      //Block(input_data.empty);
			//cs
			front = q->num;
			//cs end
      //UnBlock(input_data.full,input_data);
      printf("front : %d,  back : %d ",front,back);
			if(front == -1)
			{
				puts("Waiting for clients..");
			}
			else if(front >= back)
			{
				input_data = q->queue[back];
        
				int pid,ch;
				ch = input_data.service;
				printf("Choice of service is: %d",ch);
				pid = fork();
				if(pid == 0)
				{
					printf("\npid : %d",pid);
					if(ch == 1)
					{
						printf("\nstrings\n");
						//execl("./service1", "./service1", input_data.string,input_data.shared_mem_id,input_data.client_id, NULL);
						exit(0);
					}
					else if(ch == 2)
					{
						printf("Matrix stuff");
						//execl("./service2", "./service2", input_data.matrix[0][0],input_data.matrix[0][1],input_data.matrix[0][2],input_data.matrix[1][0],input_data.matrix[1][1],input_data.matrix[1][2],input_data.matrix[2][0],input_data.matrix[2][1],input_data.matrix[2][2],input_data.shared_mem_id,input_data.client_id, NULL);
						exit(0);
					}
					else if(ch == 3)
					{
						printf("factorial:%s", input_data.factorial);
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
				puts("\nresetting front and back");
			  back= 0;
        //Block(input_data.empty);
				//cs
				q->num = -1;
				//cs end
        //UnBlock(input_data.full,input_data);
			}
			sleep(1);
  }	
	printf("exiting sucessfully");
}