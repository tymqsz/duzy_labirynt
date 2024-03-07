#ifndef QH
#define QU

typedef struct node{
	int value;
	struct node* next;
} node_t;

typedef struct{
	int internal_size;
	int external_size;
	int internal_capacity;
	int external_capacity;
	int external_offset;
	node_t* top;
} Queue_t;

Queue_t* init_queue(int);

void push(Queue_t*, int);

int pop(Queue_t*);

void display(Queue_t*);

void destroy_queue(Queue_t* queue);

#endif
