#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

/* Nicolas Forcade-Perkins
   2-11-2022
   Lab 2 Section 4: We needed to write a progam that would create 2 processes and take an integer input from the command line. The first process would continuously print numbers [0,N/2] and the second would continuously print numbers [N/2, N]. After 5 seconds the parent would kill both processes
I was the only contributer.

*/
int main(int argc, char const * argv[]){
	if (argc != 2){
		printf("Wrong inputs");
		return -1;
	}

	pid_t child1 = fork();
	
	if(child1 == 0){ 
		int count = 0;
		int max = (atoi(argv[1]))/2;
		while(1){
			if (count > max){
				count = 0;
			}
			printf("%d\n", count);
			count ++;
			sleep(1);
		}

	}
	
	pid_t child2 = fork();
	if(child2 == 0){ //if second child
		int count = atoi(argv[1])/2;
		int max = atoi(argv[1]);
		while(1){
			if (count > max){
				count = atoi(argv[1])/2;
			}
			printf("%d\n", count);
			count ++;
			sleep(1);
		}
	}
	sleep(5);
	kill(child1, 9);
	kill(child2, 9);
	printf("Killed Children\n");
	return 1;
	
	
			
}
