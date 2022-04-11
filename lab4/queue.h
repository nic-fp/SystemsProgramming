struct node {				
       int pid;
	     char *name;
       struct node *next;
       int curr_queue_indx;
       int ops; 
};


struct queue {				
       struct node *head;
       struct node *tail ;
};

     
void enqueue(int item, char *s, struct queue *q, int curr_queue_indx) {		
     struct node *p;						

     p= (struct node *)malloc(sizeof(struct node));
     p->pid=item;
     p->name=s;	
     p->next=NULL;
     p->curr_queue_indx = curr_queue_indx; 
     p->ops =0; 
     if (q->head==NULL) q->head=q->tail=p;
     else {
          q->tail->next=p;
          q->tail=p;
     }

}



void sortedEnqueue(int item, char*s, int ops, struct queue * q, int curr_queue_indx){
  struct node *p;						
  p= (struct node *)malloc(sizeof(struct node));
  p->pid=item;
  p->name=s;	
  p->ops = ops;
  p->next=NULL;
  p->curr_queue_indx = curr_queue_indx; 
  if (q->head==NULL) q->head=q->tail=p;
  else {
    struct queue * temp = q;
    struct queue * prev; 
	  prev = (struct queue *)malloc(sizeof(struct queue));
    prev->head = NULL;
    prev->tail = NULL; 
	  
    while(temp->head->ops < ops && temp->head->next !=NULL){
      prev->head = temp->head;
      temp->head = temp->head->next;
    }
    if(prev->head == NULL){
      if(temp->head->ops > ops){
        p->next = temp->head;
        q->head = p; 
      }
      else{
        q->head->next = p; 
        q->tail = p; 
      }
    }
    else if(temp->head->next ==NULL ){
      temp->head->next = p; 
      q->tail = p; 
    }
    else{
      prev->head->next = p;
      p->next = temp->head;
    }
  }
}


int dequeue(struct queue *q) {		
    int item;				
    struct node *p;
    p = (struct node *)malloc(sizeof(struct node));
    item=q->head->pid;
    p=q->head;
    q->head=q->head->next;
    free(p);
    return item;
}

struct node dequeue_node(struct queue *q) {						
    struct node *p;
    p=q->head;
    q->head=q->head->next;
    
    return *p;
}

void delete(struct queue *q, int key)
{
     if (q->head->pid == key)
     {
        struct node *p = q->head;
        q->head = q->head->next;
        free(p);
        return;
    }
    struct node *current = q->head->next;
    struct node *previous = q->head;
    while (current != NULL && previous != NULL)
    {
      if (current->pid == key)
      {
        struct node *tmp = current;
        if (current == q->tail)
          q->tail = previous;
        previous->next = current->next;
        free(tmp);
        return;
      }
      previous = current;
      current = current->next;
    }
    return;  
  }





