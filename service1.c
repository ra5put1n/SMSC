#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <sys/msg.h> 
#include<sys/ipc.h>
#include<sys/shm.h>
#include<signal.h>

void my_handler(){}

struct ANS{
  int answer;
};

//palindrome
int isPalindrome(char str[]) { 
  int l = 0, h = strlen(str) - 1;  
  while (h > l) { 
    if (str[l] != str[h]) 
      return 0;
    else{
      l+=1;
      h-=1;
    } 
  } 
  return 1;
} 
   
int main(int argc, char * argv[]){
  printf("Hello, I am Service1 prog.   My PID is %d\n", getpid());
  signal(SIGUSR1,my_handler);
  int answer;
  int shared_address = atoi(argv[argc-2]);
  int client_id = atoi(argv[argc-1]);
  printf("\n client id : %d",client_id);
   printf("\n shmid : %d",shared_address);
  struct ANS *myans = (struct ANS *)shmat(shared_address, NULL, 0);
  answer= isPalindrome(argv[1]);
  //cs
  myans->answer = answer;
  printf("ans : %d",myans->answer);
  //cs end
  kill(client_id,SIGUSR1);
  
  printf("Service1 is exiting...\n");
  return 0;
}
