#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <sys/msg.h> 
#include<sys/ipc.h>
#include<sys/shm.h>
#include<signal.h>
#include<ctype.h>

void my_handler(){}

struct ANS{
  int answer;
	int err;
};

//palindrome
int isPalindrome(char string[]) { 
  int l = 0, h = strlen(string) - 1;  
  char str[100];
  for(int i=0;i<=h;i++)
    str[i]=tolower(string[i]);
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
  signal(SIGUSR1,my_handler);
  int answer;
  int shared_address = atoi(argv[argc-2]);
  int client_id = atoi(argv[argc-1]);
  struct ANS *myans = (struct ANS *)shmat(shared_address, NULL, 0);
  answer= isPalindrome(argv[1]);
	myans->err = 0;
  myans->answer = answer;
  kill(client_id,SIGUSR1);
  return 0;
}
