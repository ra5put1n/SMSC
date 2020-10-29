#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<stdbool.h>
#include <sys/msg.h> 
#include<string.h>

void my_handler(){}

struct ANS{
  int answer;
};


//factorial
unsigned long long int factorial(unsigned long long int n) 
{ 
  if (n == 0) 
    return 1; 
  return n * factorial(n - 1); 
}

int main(int argc, char * argv[]){
  signal(SIGUSR1,my_handler);
  //printf("Hello, I am Service3 prog. My PID is %d\n", getpid());
  int num;
  unsigned long long int answer;
  num = atoi(argv[1]);
  
  if(num<0){
    //printf("\nInvalid Input..!! Enter number >= 0. ");
    answer=0;
  }
  else
		//printf("\nNumber to find factorial of: %d",num);
    answer =factorial(num);
		//printf("\nAnswer is: %d",answer);
    int shared_address = atoi(argv[argc-2]);
    int client_id = atoi(argv[argc-1]);
		//printf("\nShared memory id: %d, client id: %d",shared_address,client_id);

    struct ANS *myans = (struct ANS *)shmat(shared_address, NULL, 0);
    myans->answer = answer;
		//detach
    //shmdt(myans);
		//printf("%d",myans->answer);
    kill(client_id,SIGUSR1);


  //printf("\nService3 is exiting...\n");
  return 0;
}
