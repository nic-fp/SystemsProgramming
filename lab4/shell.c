/*
This code is written by Nicolas Forcade-Perkins on 3-15-22.
This code an operating system. It is capable of running a FIFO, MFQ, RR, and SJF scheduling policy.
No one else contributed to this code.
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/resource.h>
#include "queue.h"

typedef struct policy {
	// Struct  describing  data associated with policy
	char* name;
	int qt;
	int num_queues;
} policy;



int fg_pid=0;
int fg_suspended=0;
int run=1;

struct queue pid_list;
struct queue scheduling_queue;
struct queue *queue_arr[11];
policy scheduling_policy = {"FIFO", 0, 1};
void childdead(int signum);


void help() {
	printf("This is manual page\n");
	printf("This shell supports the following commands:\n");
	printf("\tver\n\texec\n\tps\n\tkill\n\thelp\n\texit\n\tset_scheduling\n");
	printf("For more details please type 'help command'\n");
}

void helpcmd(char *cmd) {
	printf("This is manual page\n\n");
	if (strcmp(cmd,"ver")==0)
	{
		printf("\nver:\tShows details about the shell version\n");
	}
	else if (strcmp(cmd,"exec")==0)
	{
		printf("\nexec p1(n1,qt1) p2(n2,qt2) ...:\nExecutes the programs p1, p2 ...\nEach program types a message for n times and it is given a time quantum of qt msec.\n");
		printf("If parameter (&) is given the program will be executed in the background\n");
	}
	else if (strcmp(cmd,"ps")==0)
	{
		printf("\nps:\tShows the living process with the given pid\n");
	}
	else if (strcmp(cmd,"kill")==0)
	{
		printf("\nkill pid:\tEnds the process with the given pid\n");
	}
	else if (strcmp(cmd,"help")==0)
	{
		printf("\nhelp:\tYou should know this command by now\n");
	}
	else if (strcmp(cmd,"exit")==0)
	{
		printf("\nexit:\tEnds the experience of working in the new shell\n");
	}
	else if (strcmp(cmd,"set_scheduling")==0)
	{
		printf("\nset_scheduling:\tSets scheduling policy. Accepts FIFO, RR, MFQ, SJF\n");
	}
	else 
	{
		printf("\nNo Such command. Type help to see a list of commands\n");
	}
}

void ver() {
	printf("\nNew Shell. Works properly!!\n");
}

void ps() {
	struct node *p;
	printf("\nNEW SHELL presents the following living processes\n");
	printf("\tPID\tNAME\n");
	for (p=pid_list.head; p!=NULL; p=p->next)
	{
		printf("\t%d\t%s\n",p->pid,p->name);
	}
}

void mykill(int pid) {
	kill(pid,SIGTERM);
	printf("You have just killed process %d\n",pid );
}


void exec(char *input) {
	int i,t,status;
	char *args[10];
	char *temp;
	struct node *p;

	for (i = 0; i < 10; i++)
	{
		args[i]=(char *)malloc(10*sizeof(char));
	}

	strcpy(args[0],strtok(input,"(,"));
	for (i=1; (temp=strtok(NULL,",)"))!=NULL; i++) 
		strcpy(args[i],temp);

	if (strcmp(args[i-1],"&")==0)
	{
		args[i-1]=NULL;
	}
	else
		args[i]=NULL;
	if ((t=fork())==0)
	{
		execv(args[0],args);
	}
	
	sleep(1);
	if(strcmp(scheduling_policy.name, "FIFO") == 0){
		if (args[i-1]!=NULL)
		{
			fg_pid=t;
			kill(fg_pid,SIGCONT);
			
			while(fg_pid!=0 && fg_suspended!=1){
				pause();
			}
			
		}
	}
	else{
		if(strcmp(scheduling_policy.name, "SJF") == 0){
			enqueue(t,args[0],&pid_list,0 ); //Enqueue to pid_list then also enqueue it to a scheduluer queue
			sortedEnqueue(t,args[0], atoi(args[1]), &scheduling_queue, 0);

		}
		else{

			//Regular queueing if not SJF
			enqueue(t,args[0],&pid_list,0 ); //Enqueue to pid_list then also enqueue it to a scheduluer queue
			enqueue(t,args[0], &scheduling_queue, 0);
		}
	}
}

void scheduler(void){
	if(strcmp(scheduling_policy.name, "FIFO") == 0){
		return;
	}
	else if(strcmp(scheduling_policy.name, "RR") == 0){
		//RR
		while( scheduling_queue.head)  // Scheduling  loop2 run until queue is empty
		{
			//send  the  signal SIGCONT to  the  first  element  in  queue
			int pid = dequeue(&scheduling_queue);
			fg_pid = pid;
			kill(pid, SIGCONT);
			sleep(1); //safe delay
			usleep(scheduling_policy.qt);
			if( fg_pid != 0  && fg_suspended!=1)
			{//send  the  singal  SIGUSR1 to  the first element  in  queue
				kill(pid,SIGUSR1);
				usleep (1000) ;  //  Small  s a f e  delay	
				enqueue(pid, "p", &scheduling_queue, 0);
				//re−enqueue
			}
			else{
				printf("A child  is  dead\n");
				//remove from  l i s t & mark  child  as  dead
			}
		}
	}

	else if(strcmp(scheduling_policy.name, "MFQ") == 0){
		
		int num_queues = scheduling_policy.num_queues;
		int i; 
		int arr[num_queues];
		int counter; 
		arr[0] = 1;
		for(counter = 1; counter<num_queues; counter ++){
			arr[counter] = 0; 
		}
		for(i=0; i<num_queues; i++){
			//printf("%d", queue_arr[i].head->pid);
			if(!arr[i + 1] && i!= num_queues - 1){
				struct queue *p;						
				p = (struct queue *)malloc(sizeof(struct queue));
				p->head = NULL;
				p->tail = NULL; 
				queue_arr[i+1] = p;
			}
			while( queue_arr[i]->head)  // Scheduling  loop2 run until queue is empty
			{
				//send  the  signal SIGCONT to  the  first  element  in  queue

				struct node data = dequeue_node(queue_arr[i]);
				fg_pid = data.pid;
				int pid = data.pid; 
				int curr_q = data.curr_queue_indx;
				kill(pid, SIGCONT);
				sleep(1); //safe delay
				usleep(scheduling_policy.qt);
				if( fg_pid != 0  && fg_suspended!=1)
				{//send  the  singal  SIGUSR1 to  the first element  in  queue
					kill(pid,SIGUSR1);
					usleep (1000) ;  //  Small  s a f e  delay
					if(curr_q+1 == num_queues){	
						enqueue(pid, "p", queue_arr[i], curr_q);
					}
					else{
						enqueue(pid, "p", queue_arr[i + 1], curr_q + 1);
					}
					//re−enqueue
				}
				else{
					printf("A child  is  dead\n");
					//remove from  l i s t & mark  child  as  dead
				}
			}
		}
		
	}

	else{
		//SJF
		printf("SJF RUNNING\n");
		printf("%d\n", scheduling_queue.head->pid);
		while( scheduling_queue.head)  // Scheduling  loop2 run until queue is empty
		{
			//send  the  signal SIGCONT to  the  first  element  in  queue
			int pid = dequeue(&scheduling_queue);
			fg_pid = pid;
			kill(pid, SIGCONT);
			sleep(1); //safe delay
			while( fg_pid != 0  && fg_suspended!=1)
			{//send  the  singal  SIGUSR1 to  the first element  in  queue
				pause();
				//re−enqueue
			}
			
			printf("A child  is  dead\n");
			//remove from  l i s t & mark  child  as  dead
			
		}
		
	}
}



void myexit() {
	char yesno;
	if (pid_list.head==pid_list.tail)
	{
		run=0;
	}
	else {
		printf("There are still living processes Do you want to kill them? (y/n): ");
		yesno=getchar();
		if (yesno == 'y')
		{
			while(pid_list.head!=pid_list.tail)
			{
				mykill(pid_list.tail->pid);
				usleep(10000);
			}
			run=0;
		}
	}
}

void childdead(int signum){
	int dead_pid, status;
	
	dead_pid=wait(&status); //Isn't child already dead?
	printf("The child %d is dead\n",dead_pid );

	delete(&pid_list,dead_pid);
	printf("\n");
	if (dead_pid==fg_pid)
	{
		fg_pid=0; 

	}
}

void set_scheduling(char* policy){
	//if current policy is same as new policy do nothing
	if(strcmp(scheduling_policy.name, policy) == 0){
		return;
	}
	else if(strcmp("FIFO", policy) == 0){
		scheduling_policy.name = "FIFO";
		printf("FIFO complete\n");
	}
 	else if(strcmp("RR", policy) == 0){
		int qt; 
		printf("Enter time quanta: \n");
		scanf("%d",&qt);
		

		scheduling_policy.name = "RR";
		scheduling_policy.qt = qt;
		printf("RR complete\n");
	}
	else if(strcmp("MFQ", policy) == 0){
		int qt;
		int num_queues;
		printf("Enter time quanta: \n");
		scanf("%d",&qt);
		printf("Enter number of queues: \n");
		scanf("%d",&num_queues); 
		scheduling_policy.name = "MFQ";
		scheduling_policy.qt = qt;
		if(num_queues > 10){
			printf("Max num queues is 10. Setting to 10 queues.  \n");
			scheduling_policy.num_queues = 10 ;
		}
		else{
			scheduling_policy.num_queues = num_queues ;
		}
		printf("MFQ complete\n"); 
	}
	else if(strcmp("SJF", policy) == 0){
		scheduling_policy.name = "SJF";
		printf("SJF complete\n");
	}
	else{
		printf("No such policy. Check help for help.\n");
	}
	
}

void susp (int signum) {
	fg_suspended=1;
	printf("All processes supspended\n");
}

void cont (int signum) {
	fg_suspended=0;
	printf("Waking all processes...\n");
	while (fg_pid!=0 && fg_suspended!=1)
		pause();
}

int main(int argc, char const *argv[])
{
	char input[15][30];
	int argnum, i;

	pid_list.head=NULL;
	pid_list.tail=NULL;
	enqueue(getppid(),"NEW SHELL",&pid_list, 0);

	scheduling_queue.head = NULL;
	scheduling_queue.tail = NULL;
	queue_arr[0] = &scheduling_queue;
	signal(SIGCHLD,childdead);
	signal(SIGTSTP,susp);
	signal(SIGQUIT,cont);
	setpriority(PRIO_PROCESS,0,-20);

	ver();

	while (run){
		printf("=>");
		for (argnum=0; (scanf("%s",&input[argnum]))==1;argnum++)
			if (getchar()=='\n') break;
		if (strcmp(input[0],"ver")==0 && argnum==0) ver();
		else if (strcmp(input[0],"help")==0 && argnum==0) help();
		else if (strcmp(input[0],"help")==0 && argnum==1) helpcmd(input[argnum]);
		else if (strcmp(input[0],"ps")==0 && argnum==0) ps();
		else if (strcmp(input[0],"kill")==0 && argnum==1) mykill(atoi(input[1]));
		else if (strcmp(input[0],"set_scheduling")==0 && argnum==1) set_scheduling((input[1]));
		else if (strcmp(input[0],"exec")==0 && argnum!=0){ 
			for (i=1; i<=argnum; i++) exec(input[i]);
			scheduler();
		}
		else if (strcmp(input[0],"exit")==0 && argnum==0) myexit();
	    	else printf("No such command. Check help for help.\n");
	}

}
