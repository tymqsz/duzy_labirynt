#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "file_io.h"
#include "queue.h"
#include "metadata.h"

void reload_parent(int**, int*, int*, int, int, int);

void reload_graph(int**, int*, int*, int, int, int);

void reconstruct_path(int start, int end, point_t true_size);

void traverse(int start_node, int end_node, point_t true_size){
	int n_nodes = true_size.x*true_size.y;
	int HELD_PARENTS = 60000;
	int HELD_NODES = 60000;
	int min_node = 0, max_node = HELD_NODES;
	int min_parent = 0, max_parent = HELD_PARENTS;
	if(n_nodes < HELD_PARENTS)
		HELD_PARENTS = n_nodes;
	if(n_nodes < HELD_NODES)
		HELD_NODES = n_nodes;

	Queue_t* queue = init_queue(INTERNAL_QUEUE_SIZE, EXTERNAL_QUEUE_SIZE);

	int node = start_node, next;
	int EMPTY = true_size.x*true_size.y + 1;

	init_file_vector(PARENT_BIN, n_nodes, EMPTY);
	update_file_vector(PARENT_BIN, node, node);
	push(queue, node);
	
	int* parent = read_file_vector(PARENT_BIN, node, HELD_PARENTS);
	int* graph = read_file_vector(GRAPH_BIN, 0, 4*HELD_NODES);
	
	while(queue->internal_size > 0){
		node = pop(queue);
		
		if(node == end_node){
			printf("sciezka znaleziona\n");
			break;
		}

		reload_graph(&graph, &min_node, &max_node, node, n_nodes, HELD_NODES);

		for(int i = 0; i < 4; i++){
			if(graph[(node-min_node)*4+i] != -1){
				next = graph[(node-min_node)*4+i];
				
				reload_parent(&parent, &min_parent, &max_parent, next, n_nodes, HELD_PARENTS);
				
				if (parent[next-min_parent] == EMPTY){
					update_file_vector(PARENT_BIN, next, node);
					parent[next-min_parent] = node;
					push(queue, next);
				}
			}
		}
	}

	free(graph);
	free(parent);

	reconstruct_path(start_node, end_node, true_size);
}

int get_dir_index(point_t dir){
	if(dir.x == -1)
		return 0;
	if(dir.x == 1)
		return 2;
	if(dir.y == -1)
		return 1;
	if(dir.y == 1)
		return 3;
}

void reconstruct_path(int start, int end, point_t true_size){
	int crt = end;
	int read;
	 
	init_file_vector(PATH_BIN, true_size.x*true_size.y, -1);

	int node = 0;
	while(crt != start){
		read = read_file_position(PARENT_BIN, crt);
		 
		update_file_vector(PATH_BIN, node, crt);
		
		crt = read;
		node++;
	}	
	update_file_vector(PATH_BIN, node, crt);
	
	int x, y, prev_x, prev_y, steps = 0;
	point_t dir;
	int dir_index, prev_dir_index = -1;
	char turn[20];
	FILE* f = fopen("path.txt", "w");
	
	crt = read_file_position(PATH_BIN, node);
	prev_x = crt % true_size.x;
	prev_y = crt / true_size.x;
	node--;
	while(node >= 0){
		crt = read_file_position(PATH_BIN, node);
		
		x = crt % true_size.x;
		y = crt / true_size.x;
		
		dir.x = x - prev_x;
		dir.y = y - prev_y;
		
		dir_index = get_dir_index(dir);
		
		if(prev_dir_index == -1 || prev_dir_index == dir_index)
			steps++;
		else{
			if(dir_index - prev_dir_index == 1 || dir_index - prev_dir_index == -3)
				strcpy(turn, "TURNRIGHT");
			else
				strcpy(turn, "TURNLEFT");
			
			fprintf(f, "FORWARD %d\n%s\n", steps, turn);
			steps = 1;
		}

		prev_dir_index = dir_index;
		prev_x = x;
		prev_y = y;

		node--;
	}
	fprintf(f, "FORWARD %d\n", steps);
	fclose(f);
}

void reload_parent(int** parent, int* min_parent, int* max_parent, int crt, int n_nodes, int HELD_PARENTS){
		if(crt < *min_parent || crt >= *max_parent){
			*min_parent = max(0, crt - HELD_PARENTS/2);
			*max_parent = min(n_nodes, *min_parent+HELD_PARENTS);
			
			free(*parent);
			
			*parent = read_file_vector(PARENT_BIN, *min_parent, *max_parent-*min_parent);
		}
}

void reload_graph(int** graph, int* min_node, int* max_node, int crt, int n_nodes, int HELD_NODES){
		if(crt < *min_node || crt >= *max_node){
			*min_node = max(0, crt-HELD_NODES/2);
			*max_node = min(*min_node+HELD_NODES, n_nodes);

			free(*graph);

			*graph = read_file_vector(GRAPH_BIN, 4*(*min_node), (*max_node-*min_node)*4);
		}
}
