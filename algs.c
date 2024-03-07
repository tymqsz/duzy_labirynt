#include <stdlib.h>
#include <stdio.h>

#include "data.h"
#include "file_io.h"
#include "queue.h"

void reconstruct_path(int start, int end);

void load_proper_block(point_t* crt, box_t* boxes, char* filename, char** lab, int* box){
	if(crt->x > boxes[*box].A.x && crt->x < boxes[*box].B.x && crt->y > boxes[*box].A.y && crt->y < boxes[*box].B.y)
		return;

	for(int i = 0; i < 9; i++){
		if((crt->x > boxes[i].A.x && crt->y > boxes[i].A.y && crt->y < boxes[i].B.y && crt->x < boxes[i].B.x)){
			file_to_vec(filename, lab, &boxes[i]);
			*box = i;
			return;
		}
	}
	
	fprintf(stderr, "Couldnt load a proper block\n");
	exit(1);
}

void extract_nodes(char** lab, point_t size, point_t start, point_t end, box_t* boxes, char* filename, int box){
	int* prev_x = malloc(sizeof(int) * (size.x*2+1));
	int* prev_y = malloc(sizeof(int) * (size.y*2+1));
	
	for(int i = 0; i < size.x; i++)
		prev_x[i] = -1;
	for(int i = 0; i < size.y; i++)
		prev_y[i] = -1;
	
	char NEIGH_FILE[] = "neigh.bin";
	int N = size.x*size.y*4;
	int EMPTY = size.x*size.y;
	init_array_binary(NEIGH_FILE, N, EMPTY);
	
	char TRANSLATE[] = "trans.bin";
	init_array_binary(TRANSLATE, N/2, -1);
	update_array_binary(TRANSLATE, 0, 1);
	update_array_binary(TRANSLATE, 0, 1);
	point_t crt;
	int act_x, act_y;
	int node = 0;
	int neigh_left, neigh_down;
	for(int y = 1; y < size.y*2+1; y += 2){
		for(int x = 1; x < size.x*2+1; x += 2){
			crt.x = x;
			crt.y = y;
			load_proper_block(&crt, boxes, filename, lab, &box);
			
			act_y = crt.y-boxes[box].A.y;
			act_x = crt.x-boxes[box].A.x;

			if(!((lab[act_y][act_x-1] && lab[act_y][act_x+1] && !lab[act_y-1][act_x] && !lab[act_y+1][act_x]) ||
				(!lab[act_y][act_x-1] && !lab[act_y][act_x+1] && lab[act_y-1][act_x] && lab[act_y+1][act_x])) ||
				(start.x == x && start.y == y) || (end.x == x && end.y == y)){

				if(node != 0){
					if(lab[act_y][act_x-1] && prev_y[y] != -1){
						update_array_binary(NEIGH_FILE, node*4, prev_y[y]-node);
							
						neigh_left = read_digit_binary(NEIGH_FILE, prev_y[y]*4+1); 
						if(neigh_left == EMPTY){
							update_array_binary(NEIGH_FILE, prev_y[y]*4+1, node-prev_y[y]);
						}
					}
					if(lab[act_y-1][act_x] && prev_x[x] != -1){
						update_array_binary(NEIGH_FILE, node*4+2, prev_x[x]-node);
						neigh_down = read_digit_binary(NEIGH_FILE, prev_x[x]*4+3); 
						if(neigh_down == EMPTY){
							update_array_binary(NEIGH_FILE, prev_x[x]*4+3, node-prev_x[x]);
						}
					}
				}
				
				update_array_binary(TRANSLATE, node*2, x);
				update_array_binary(TRANSLATE, node*2+1, y);
				prev_x[x] = node;
				prev_y[y] = node;
				node++;
			}
		}
	}
	
	free(prev_x);
	free(prev_y);
	update_array_binary(NEIGH_FILE, size.x*size.y*4-1, node);
}

void bfs(int start, int end, int n_nodes, point_t size){
	Queue_t* queue = init_queue(1000);
	
	int HELD_NODES = 1000;
	if(HELD_NODES > n_nodes)
		HELD_NODES = n_nodes;

	int* nodes = read_array_binary("neigh.bin", 0, 4*HELD_NODES);
	int min_node = 0, max_node = HELD_NODES;
	
	char PARENT[] = "parent.bin";
	init_array_binary(PARENT, n_nodes, -1);
	
	int crt = start;
	int vis;

	push(queue, crt);
	update_array_binary(PARENT, crt, crt);
	while(queue->internal_size != 0 || queue->external_size != 0){
		crt = pop(queue);
		if(crt == end){
			printf("found a way out\n");
		}
		if(crt >= max_node){
			max_node = min(crt+HELD_NODES/2, n_nodes);
			min_node = max(max_node-HELD_NODES, 0);
			
			free(nodes);
			nodes = read_array_binary("neigh.bin", 4*min_node, 4*(max_node-min_node));
		}
		if(crt < min_node){
			min_node = max(crt-HELD_NODES/2, 0);
			max_node = min(min_node+HELD_NODES, n_nodes);

			free(nodes);
			nodes = read_array_binary("neigh.bin", 4*min_node, 4*(max_node-min_node));
		}

		for(int i = 0; i < 4; i++){
			int next;
			next = crt + nodes[(crt-min_node)*4+i];
			if(next-crt != size.x*size.y){
				vis = read_digit_binary(PARENT, next);
				if(vis == -1){
					update_array_binary(PARENT, next, crt);
					push(queue, next);
				}
			}
		}
	}
		
	reconstruct_path(0, n_nodes-1);		
}

int get_dir(int Ax, int Ay, int Bx, int By){
	if(Bx-Ax < 0)
		return 0;
	if(By-Ay < 0)
		return 1;
	if(Bx-Ax > 0)
		return 2;
	return 3;
}

void reconstruct_path(int start, int end){
	char PARENT[] = "parent.bin";
	char TRANSLATE[] = "trans.bin";
	
	int crt = end;
	int read;
	int x, y;
	int* path = malloc(2*end*sizeof(int));
	for(int k = 0; k < 2*end; k++)
		path[k] = -1;
	
	int i = 0;
	while(crt != start){
		x = read_digit_binary(TRANSLATE, crt*2);
		y = read_digit_binary(TRANSLATE, crt*2+1);

		path[i*2] = x;
		path[i*2+1] = y;
		i++;

		read = read_digit_binary(PARENT, crt);

		crt = read;
	}
	x = read_digit_binary(TRANSLATE, start*2);
	y = read_digit_binary(TRANSLATE, start*2+1);
	
	path[i*2] = x;
	path[i*2+1] = y;
	
	FILE* out = fopen("path.txt", "w");
	int l = end-1;
	while(path[l*2] == -1)
		l--;
	while(l >= 0){
		x = path[l*2];
		y = path[l*2+1];
		fprintf(out, "(%d, %d)\n", x, y);
		l--;
	}
	free(path);


}
