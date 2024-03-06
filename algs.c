#include <stdlib.h>
#include <stdio.h>

#include "data.h"
#include "file_io.h"

int REC_DEPTH = 0;

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

				int* array = read_array_binary("neigh.bin", 0, 9*4);
				
				free(array);
				prev_x[x] = node;
				prev_y[y] = node;
				node++;
			}
		}
	}
	
	update_array_binary(NEIGH_FILE, size.x*size.y*4-1, node);
}


void traverse2(char** lab, char* INPUT, box_t* boxes, int box, point_t start, point_t end, point_t size){
	char QUEUE[] = "queue.txt";
	init_queue(QUEUE);
	int TOP_LINE = 0;
	int SIZE = 1;
	append(QUEUE, start);

	char VISITED[] = "visited.txt";
	init_visited_file(VISITED, size);
	
	point_t crt = start;
	change_file_position(VISITED, &size, &crt, 1);
	while(SIZE != 0){
		crt = top(QUEUE, TOP_LINE);
		SIZE--;
		if(crt.x == end.x && crt.y == end.y){
			printf("found a way out (%d, %d)\n", end.x, end.y);
			exit(EXIT_SUCCESS);
		}
		TOP_LINE++;
		
		load_proper_block(&crt, boxes, INPUT, lab, &box);


		if (lab[crt.y - boxes[box].A.y][crt.x - 1 - boxes[box].A.x]) {
			if (crt.x >= 2) {
				point_t* nxt = malloc(sizeof(point_t));
				nxt->x = crt.x - 2;
				nxt->y = crt.y;
				if (read_file_position(VISITED, &size, nxt) == 0){
					SIZE++;
					change_file_position(VISITED, &size, nxt, 1);
					append(QUEUE, *nxt);
				}
				free(nxt);
			}
		}
		if (lab[crt.y - boxes[box].A.y][crt.x + 1 - boxes[box].A.x]) {
			if (crt.x < size.x) {
				point_t* nxt = malloc(sizeof(point_t));
				nxt->x = crt.x + 2;
				nxt->y = crt.y;
				if (read_file_position(VISITED, &size, nxt) == 0){
					SIZE++;
					append(QUEUE, *nxt);
					change_file_position(VISITED, &size, nxt, 1);
				}
				free(nxt);
			}
		}
		if (lab[crt.y - 1 - boxes[box].A.y][crt.x - boxes[box].A.x]) {
			if (crt.y >= 2) {
				point_t* nxt = malloc(sizeof(point_t));
				nxt->x = crt.x;
				nxt->y = crt.y - 2;
				if (read_file_position(VISITED, &size, nxt) == 0){
					SIZE++;
					append(QUEUE, *nxt);
					change_file_position(VISITED, &size, nxt, 1);
				}
				free(nxt);
			}
		}
		if (lab[crt.y + 1 - boxes[box].A.y][crt.x - boxes[box].A.x]) {
			if (crt.y < size.y) {
				point_t* nxt = malloc(sizeof(point_t));
				nxt->x = crt.x;
				nxt->y = crt.y + 2;
				if (read_file_position(VISITED, &size, nxt) == 0){
					SIZE++;
					append(QUEUE, *nxt);
					change_file_position(VISITED, &size, nxt, 1);
				}
				free(nxt);
			}
		}
	}	
}


void traverse(char** lab, char* visited, char* filename, int* box, box_t* boxes, point_t* size, point_t* end, point_t* crt){
	if (crt->x == end->x && crt->y == end->y) {
		printf("found a way out\n");
		exit(EXIT_SUCCESS);
	}
	
	change_file_position(visited, size, crt, 1);

	// make a loop (moving vectors)
	load_proper_block(crt, boxes, filename, lab, box);
	if (lab[crt->y - boxes[*box].A.y][crt->x - 1 - boxes[*box].A.x]) {
		if (crt->x >= 2) {
			crt->x -= 2;
			if (read_file_position(visited, size, crt) == 0) {
				traverse(lab, visited, filename, box, boxes, size, end, crt);
			}
			crt->x += 2;
		}
	}

	load_proper_block(crt, boxes, filename, lab, box);
	if (lab[crt->y - boxes[*box].A.y][crt->x + 1 - boxes[*box].A.x]) {
		if (crt->x + 2 < size->x) {
			crt->x += 2;
			if (read_file_position(visited, size, crt) == 0)
				traverse(lab, visited, filename, box, boxes, size, end, crt);
			crt->x -= 2;
		}
	}

	load_proper_block(crt, boxes, filename, lab, box);
	if (lab[crt->y - 1 - boxes[*box].A.y][crt->x - boxes[*box].A.x]) {
		if (crt->y >= 2) {
			crt->y -= 2;

			if (read_file_position(visited, size, crt) == 0)
				traverse(lab, visited, filename, box, boxes, size, end, crt);
			crt->y += 2;
		}
	}

	load_proper_block(crt, boxes, filename, lab, box);
	if (lab[crt->y + 1 - boxes[*box].A.y][crt->x - boxes[*box].A.x]) {
		if (crt->y + 2 < size->y) {
			crt->y += 2;
			if (read_file_position(visited, size, crt) == 0)
				traverse(lab, visited, filename, box, boxes, size, end, crt);
        crt->y -= 2;
		}
	}
}
