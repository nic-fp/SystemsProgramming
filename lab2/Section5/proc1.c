#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

/* Nicolas Forcade-Perkins
   2-1-2022
   Lab 2 Section 5: We needed to write a progam that spawned 2 processes. After 3 seconds the parent would send the pid of P1 to P0. P0 would then send the SIGTERM signal to P1 which would then print a message and exit. P0 would then send a message back to the parent. The parent would receive this message and exit.
I was the only contributer.

*/

void sigterm(){
	printf("P1 exiting\n");
	exit(1);
}

int main(int argc, char const * argv[]){
	int fd[4];
	char*s;
	char* buf[1024];
	pipe(fd+0);
	pipe(fd+2);
	pid_t child1 = fork();
	pid_t child2;
	if(child1 == 0){ 
		read(fd[0], &child2, sizeof(child2));
		printf("Read\n");
		kill(child2, SIGTERM);
		sleep(1);
		kill(child2, SIGKILL);
		s = "Test String";
		write(fd[3], s, strlen(s));
		exit(1);
		
	}
	else{
		child2 = fork();
		if (child2 == 0){
			signal(SIGTERM, sigterm);
			while(1){
				;
			}		
		}
		else{	
			sleep(3);
			printf("writing\n");
			write(fd[1], &child2, sizeof(child2));
			read(fd[2], buf, 12);
		
			printf("%s\n", buf);
			close(fd[0]);
			close(fd[1]);
			close(fd[2]);
			close(fd[3]);			
			exit(1);
		}
		
	}
	

	return 1;

}



