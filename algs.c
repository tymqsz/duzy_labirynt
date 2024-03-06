#include <stdlib.h>
#include <stdio.h>

#include "data.h"
#include "file_io.h"

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
	
	point_t crt;
	int act_x, act_y;
	int node = 0;
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
							
						int* neigh_left = read_array_binary(NEIGH_FILE, prev_y[y]*4+1, 1); 
						if(*neigh_left == EMPTY){
							update_array_binary(NEIGH_FILE, prev_y[y]*4+1, node-prev_y[y]);
						}
						free(neigh_left);
					}
					if(lab[act_y-1][act_x] && prev_x[x] != -1){
						update_array_binary(NEIGH_FILE, node*4+2, prev_x[x]-node);
						int* neigh_down = read_array_binary(NEIGH_FILE, prev_x[x]*4+3, 1); 
						if(*neigh_down == EMPTY){
							update_array_binary(NEIGH_FILE, prev_x[x]*4+3, node-prev_x[x]);
						}
						free(neigh_down);
					}
				}

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
