
/*
This code is written by Nicolas Forcade-Perkins on 2-21-22.
This code emulates a timeout feature.
No one else contributed to this code.
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>


int flag = 0;
void stop(int sig_num){
	flag = 1;
}



int main(int argc, char* argv[] ){
    int timeout = atoi(argv[1]);
    printf( "Message  from  father :  Creating  program %s\n" ,  argv[2]);
	pid_t pid = fork();
	if(pid == 0){ // if child
        execl(argv[2], argv[2], NULL); 
        // I think execl will make it so that the rest of the code doesn't run for child
    } 
    signal(SIGALRM, stop);
    alarm(timeout);
    while(!flag){
        ;
    }
    kill(pid, 9);

}