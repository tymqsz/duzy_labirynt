#ifndef QH
#define QU


/* struktura zawierajaca informacje 
   o elemencie kolejki */
typedef struct node{
	int value;
	struct node* next;
} node_t;

typedef struct{
	int internal_size; /* rozmiar kolejki w pamieci RAM */
	int external_size; /* rozmiar kolejki w pamieci ROM */
	int internal_capacity; /* pojemnosc kolejki w pamieci RAM */
	int external_capacity; /* pojemnosc kolejki w pamieci ROM */
	int external_offset; /* indeks pierwszego elementu w pamieci ROM */
	node_t* top;
} Queue_t;

Queue_t* init_queue(int, int);

void push(Queue_t*, int);

int pop(Queue_t*);

void display(Queue_t*);

void destroy_queue(Queue_t* queue);

#endif