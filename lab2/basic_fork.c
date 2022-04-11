#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>




int main(int argc, char const *argv[])
{
	
	printf("Now I will try to create a child\n");
	if (fork()==0)
	{
		printf("Hello, I am a child with PID: %d\n", getpid());
		exit(0);
	}
	printf("Father is done, my PID is %d\n",getpid());
	
	return 0;
}

