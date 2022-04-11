/*
This code is written by Nicolas Forcade-Perkins on 2-21-22.
This code emulates a sjf scheduler.
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



int child_flag = 0; 

typedef struct node {
	// Struct  describing  the  node  of  the  queue
	pid_t data;
	struct node* next;
	struct node* prev;
} node;


typedef struct queue {
	node* head;
	node* tail;
} queue;


void  enqueue( queue* q, pid_t data){
	node* temp;

	temp = (node*)malloc(sizeof(node));

	if(!temp){
		return ;
	}
	temp->data = data;

	temp->next = q->tail;
	if(!(q->head)){
		q->head = temp;
		q->tail = temp;
		temp->next = NULL;
		temp->prev = NULL; 
		return ;
	}
	else{
		q->tail = temp;
		temp->next->prev = temp;
		temp->prev = NULL; 
	}

	return ;
};

pid_t  dequeue(queue* q){
	pid_t data;
	if(!q){
		return NULL;
	}
	if(!(q->tail) && !(q->head)){
		return NULL;  
	}
	else if(!(q->tail->prev) && !(q->tail->next)){
		data = q->head->data;
		q->head = NULL;  
		return data;
	}
	else{
		data = q->head->data; 
		node* temp = q->head; 
		q->head = q->head->prev; 
		q->head->next = NULL;
		free(temp);
		return data;
	}

	
};

typedef struct converter{
    int number;
    char* proc[20];
} converter; 

void swap(converter *x, converter *y)
{
    converter temp = *x;
    *x = *y;
    *y = temp;
}

void sorter(converter c[], int num_prog){
    
    for(int i = 0; i< num_prog -1; i++){
        
        for(int j= 0; j<num_prog-i-1; j++){
            
            if(c[j].number > c[j+1].number){
                swap(&c[j], &c[j+1]);
            }
        }
    }
};


void  termchild(int sig_num){
	// Signal  hanlder ,  the  termination  of  a  process. . .
	signal(SIGCHLD,termchild);
	child_flag = 1; 
	}

int main(int argc, char* argv[] ){
	int qt; 
	int num_prog;
	if(argc > 2){
		num_prog = argc ; // minus two will happen in for loop starting at 2 
		qt = 1000 * atoi(&argv[1][0]) ; // Here you need the qt
	}
	else{
		printf("Not enough arguments\n");
		exit(-1);
	}
    
    

    

	queue q;
    queue q2;
	q.head = q.tail = NULL;
	q2.head = q2.tail = NULL; 
    char * proc[num_prog -2]; 
    converter c[num_prog-2];
    for(int i = 0; i<num_prog-2; i++ ){
        proc[i] = argv[i+2];
        converter c1; 
        strcpy(c1.proc, proc[i]);
        proc[i][0] = '0';
        
        int x = atoi(proc[i]);
        
        c1.number = x;
        
        
        
        
        c[i] = c1; 
    }

    sorter(c, num_prog-2);
    
    
	
	for( int i = 0; i< num_prog-2; i++){
		printf( "Message  from  father :  Creating  program %s\n" ,  c[i].proc);
		pid_t pid = fork();
		if(pid == 0){ // if child
			execl(c[i].proc, c[i].proc, NULL); 
			// I think execl will make it so that the rest of the code doesn't run for child
		} 
		else{
			enqueue(&q, pid);
		}
		
	}
	sleep(1) ;  
	//  Small  safe  delay
	printf( "\nI am the  Scheduler  and  I  will now begin scheduling my programs :\n" ) ;
	signal(SIGCHLD, termchild);
	// PART 3:  Scheduling  loop
	while( q.head)  // Scheduling  loop2 run until queue is empty
	{
	//send  the  signal SIGCONT to  the  first  element  in  queue
		pid_t pid = dequeue(&q);
		kill(pid, SIGCONT);
		while( !(child_flag) )
		{
            ; 
		}
		
        child_flag = 0; 
        printf("A child  is  dead\n");
        //remove from  l i s t & mark  child  as  dead
    
	}
    
}

