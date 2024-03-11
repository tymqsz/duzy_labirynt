#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "file_io.h"

#define QUEUE "queue.bin"

/* implementacja kolejki czesciowo 
   przechowywanej w pliku binarnym */

Queue_t* init_queue(int internal_capacity, int external_capacity){
	Queue_t* new = malloc(sizeof(Queue_t));

	if(new == NULL){
		printf("cannot create queue\n");
		exit(EXIT_FAILURE);
	}

	new->internal_size = 0;
	new->internal_capacity = internal_capacity; /* maksymalna liczba intow przechowywanych
												   w kolejce w pamieci RAM */
	new->external_size = 0;
	new->external_capacity = external_capacity; /* maksymalna liczba intow przechowywanch
	                                               w kolejce w pamieci ROM */
	new->external_offset = 0; /* indeks pierwszego elementu w pamieci ROM */	
	new->top = NULL;
	
	init_file_vector(QUEUE, new->external_capacity, 0);

	return new;
}

void push(Queue_t* queue, int value){
	/* jesli rozmiar wewnetrzny przekroczony
       zapisz w element w pliku */
	if(queue->internal_size >= queue->internal_capacity){
		if(queue->external_size >= queue->external_capacity){
			printf("cannot push\n");
			exit(EXIT_FAILURE);
		}
		
		update_file_vector(QUEUE, queue->external_size, value);

		queue->external_size++;
		return;
	}
	
	/* dodanie elementu do kolejki */
	node_t* new = malloc(sizeof(node_t));
	if(new == NULL){
		printf("cannot create node\n");
		exit(EXIT_FAILURE);
	}
	
	new->value = value;
	new->next = NULL;
	
	if(queue->internal_size == 0)
		queue->top = new;
	else{
		node_t* crt = queue->top;
		while(crt->next != NULL)
			crt = crt->next;
		crt->next = new;
	}
	
	queue->internal_size++;
}

int pop(Queue_t* queue){
	/* jesli liczba elementow w kolejce w RAM = 0,
	   sprobuj czytac z pliku */
	if(queue->internal_size == 0){
		if(queue->external_size == 0){
			printf("no items in queue\n");
			return -1;
		}
		
		int reload_cnt = queue->external_size;
		if(reload_cnt > queue->internal_capacity)
			reload_cnt = queue->internal_capacity;
		
		/* czytanie i zapisywanie elemetow z pliku
		   do pamieci wewnetrznej */
		int entry;
		for(int i = 0; i < reload_cnt; i++){
			entry = read_file_position(QUEUE, queue->external_offset+i);
			push(queue, entry);
		}
		
		queue->external_size -= reload_cnt;
		queue->external_offset += reload_cnt; /* przesuniecie indeksu poczatkowego
												 kolejki w pliku */
	}
	
	/* czytanie elemetu z kolejki wewnetrznej */
	node_t* new_top = queue->top->next;
	
	int value = queue->top->value;

	free(queue->top);

	queue->top = new_top;
	queue->internal_size--;

	return value;
}


void destroy_queue(Queue_t* queue){
	while(queue->internal_size > 0)
		pop(queue);
	

	free(queue);
}

	
