#include <stdlib.h>
#include <stdio.h>

#include "metadata.h"
#include "data.h"
#include "file_io.h"
#include "queue.h"

void reconstruct_path(int start, int end);

void load_proper_block(point_t* crt, block_t* blocks, char* filename, char** lab, int *block_index){
	if(crt->x > blocks[*block_index].mini.x && crt->x < blocks[*block_index].maxi.x &&
	   crt->y > blocks[*block_index].mini.y && crt->y < blocks[*block_index].maxi.y)
		return;

	for(int i = 0; i < 9; i++){
		if(crt->x > blocks[i].mini.x && crt->x < blocks[i].maxi.x &&
		   crt->y > blocks[i].mini.y && crt->y < blocks[i].maxi.y){
			lab_to_vec(filename, lab, blocks[i]);
			*block_index = i;
			return;
		}
	}
	
	fprintf(stderr, "Couldnt load a proper block\n");
	exit(EXIT_FAILURE);
}

void extract_nodes(char** lab, point_t size, point_t start, point_t end, block_t* blocks, char* filename, int block_index){
	int* prev_x = malloc(sizeof(int) * (size.x*2+1));
	int* prev_y = malloc(sizeof(int) * (size.y*2+1));
	for(int i = 0; i < size.x; i++)
		prev_x[i] = -1;
	for(int i = 0; i < size.y; i++)
		prev_y[i] = -1;
	

	int EMPTY = size.x*size.y+1;
	init_file_vector(GRAPH_BIN, size.x*size.y*4, EMPTY);
	
	init_file_vector(NODE_COORDS_BIN, size.x*size.y*2, -1);
	update_file_vector(NODE_COORDS_BIN, 0, 1);

	point_t crt = {1, 1};
	point_t crt_index = {1, 1};
	int node_nr = 0;
	int neigh_left, neigh_down;
	while(crt.y < size.y*2+1){
		while(crt.x < size.x*2+1){
			load_proper_block(&crt, blocks, filename, lab, &block_index);
			
			crt_index.x = crt.x - blocks[block_index].mini.x;
			crt_index.y = crt.y - blocks[block_index].mini.y;

			if(!((lab[crt_index.y][crt_index.x-1] && lab[crt_index.y][crt_index.x+1] && !lab[crt_index.y-1][crt_index.x] && !lab[crt_index.y+1][crt_index.x]) ||
				(!lab[crt_index.y][crt_index.x-1] && !lab[crt_index.y][crt_index.x+1] && lab[crt_index.y-1][crt_index.x] && lab[crt_index.y+1][crt_index.x])) ||
				(start.x == crt.x && start.y == crt.y) || (end.x == crt.x && end.y == crt.y)){
				if(node_nr != 0){
					if(lab[crt_index.y][crt_index.x-1] && prev_y[crt.y] != -1){
						update_file_vector(GRAPH_BIN, node_nr*4, prev_y[crt.y]-node_nr);
						neigh_left = read_file_position(GRAPH_BIN, prev_y[crt.y]*4+1); 
						
						if(neigh_left == EMPTY){
							update_file_vector(GRAPH_BIN, prev_y[crt.y]*4+1, node_nr-prev_y[crt.y]);
						}
					}
					if(lab[crt_index.y-1][crt_index.x] && prev_x[crt.x] != -1){
						update_file_vector(GRAPH_BIN, node_nr*4+2, prev_x[crt.x]-node_nr);
						neigh_down = read_file_position(GRAPH_BIN, prev_x[crt.x]*4+3); 
						
						if(neigh_down == EMPTY){
							update_file_vector(GRAPH_BIN, prev_x[crt.x]*4+3, node_nr-prev_x[crt.x]);
						}
					}
				}
				
				update_file_vector(NODE_COORDS_BIN, node_nr*2, crt.x);
				update_file_vector(NODE_COORDS_BIN, node_nr*2+1, crt.y);
				prev_x[crt.x] = node_nr;
				prev_y[crt.y] = node_nr;
				node_nr++;
			}
			crt.x += 2;
		}
		crt.x = 1;
		crt.y += 2;
	}
	
	free(prev_x);
	free(prev_y);
	update_file_vector(GRAPH_BIN, size.x*size.y*4-1, node_nr);
}

void bfs(int start, int end, int n_nodes, point_t size){
	Queue_t* queue = init_queue(INTERNAL_QUEUE_SIZE, EXTERNAL_QUEUE_SIZE);
	
	int HELD_NODES = MAX_N_HELD_NODES;
	if(HELD_NODES > n_nodes)
		HELD_NODES = n_nodes;
	
	int* nodes = read_file_vector(GRAPH_BIN, 0, 4*HELD_NODES);
	int min_node = 0, max_node = HELD_NODES;
	
	init_file_vector(PARENT_BIN, n_nodes, -1);
	
	int crt = start;
	int vis, next;
	push(queue, crt);
	update_file_vector(PARENT_BIN, crt, crt);
	while(queue->internal_size != 0 || queue->external_size != 0){
		crt = pop(queue);
		if(crt == end){
			printf("found a way out\n");
		}
		if(crt >= max_node){
			max_node = min(crt+HELD_NODES/2, n_nodes);
			min_node = max(max_node-HELD_NODES, 0);
			
			free(nodes);
			nodes = read_file_vector(GRAPH_BIN, 4*min_node, 4*(max_node-min_node));
		}
		if(crt < min_node){
			min_node = max(crt-HELD_NODES/2, 0);
			max_node = min(min_node+HELD_NODES, n_nodes);

			free(nodes);
			nodes = read_file_vector(GRAPH_BIN, 4*min_node, 4*(max_node-min_node));
		}

		for(int i = 0; i < 4; i++){
			next = crt + nodes[(crt-min_node)*4+i];
			if(next-crt != size.x*size.y+1){
				vis = read_file_position(PARENT_BIN, next);
				if(vis == -1){
					update_file_vector(PARENT_BIN, next, crt);
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
	int crt = end;
	int read;
	int x, y;
	int* path = malloc(2*end*sizeof(int));
	for(int k = 0; k < 2*end; k++)
		path[k] = -1;
	
	int i = 0;
	while(crt != start){
		x = read_file_position(NODE_COORDS_BIN, crt*2);
		y = read_file_position(NODE_COORDS_BIN, crt*2+1);

		path[i*2] = x;
		path[i*2+1] = y;
		i++;

		read = read_file_position(PARENT_BIN, crt);

		crt = read;
	}
	x = read_file_position(NODE_COORDS_BIN, start*2);
	y = read_file_position(NODE_COORDS_BIN, start*2+1);
	
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
