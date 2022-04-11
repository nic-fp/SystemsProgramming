#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

void cont(int sig_num){
	signal(SIGCONT,cont);
	printf("Process %d starts...\n",getpid());
}

void stop(int sig_num){
	signal(SIGUSR1,stop);
	printf("Process %d paused...\n",getpid());
	pause();
}

int main (int argc, char *argv[]) {

	int i,num,sltime;
	signal(SIGUSR1,stop);
	signal(SIGCONT,cont);

	setpriority(PRIO_PROCESS,0,-20);
	num =atoi(argv[1]);
	sltime = 1000*atoi(argv[2]);
	pause();
	for (i=1; i<=num; i++){
		printf("This is program %s and it prints for the %d time of %d...\n",argv[0],i,num);
		usleep(sltime);
	}
}
