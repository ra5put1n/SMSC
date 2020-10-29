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

//matrix
double determinant(double arr[3][3]){
  double det = 0;
  for(int i=0;i<3;i++)
      det = det + (arr[0][i]*(arr[1][(i+1)%3]*arr[2][(i+2)%3] - arr[1][(i+2)%3]*arr[2][(i+1)%3]));
  return det;
}

int main(int argc, char * argv[]){
  double arr[3][3],answer;
  //printf("Hello, I am Service2 : Matrix Determinant program. My PID is %d\n", getpid());
  signal(SIGUSR1,my_handler);
  if(argc-3!=9){
    printf("Invalid Inputs..!!");
    exit(0);
  }
  else{
    for(int i=1;i<argc-2;i++)
      for(int j=0;j<3;j++)
        for(int k=j;k<3;k++)
          arr[j][k] = atoi(argv[i]);

    int shared_address = atoi(argv[argc-2]);
    int client_id = atoi(argv[argc-1]);

    struct ANS *myans = (struct ANS *)shmat(shared_address, NULL, 0);
    answer=determinant(arr);
    myans->answer = answer;
    kill(client_id,SIGUSR1);
  }
  //printf("Service2 is exiting...\n");
  return 0;
}
