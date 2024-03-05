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
	int* prev_x = malloc(sizeof(int) * size.x);
	int* prev_y = malloc(sizeof(int) * size.y);
	
	for(int i = 0; i < size.x; i++)
		prev_x[i] = -1;
	for(int i = 0; i < size.y; i++)
		prev_y[i] = -1;

	int N = size.x*size.y;
	int** neigh = calloc(N, sizeof(int*));
	for(int i = 0; i < N; i++){
		neigh[i] = calloc(4, sizeof(int));
		for(int j = 0; j < 4; j++)
			neigh[i][j] = 9999999;
	}

	point_t crt;
	int act_x, act_y;
	int node = 0;
	for(int y = 1; y < size.y; y += 2){
		for(int x = 1; x < size.x; x += 2){
			crt.x = x;
			crt.y = y;
			load_proper_block(&crt, boxes, filename, lab, &box);
			
			act_y = crt.y-boxes[box].A.y;
			act_x = crt.x-boxes[box].A.x;

			if((lab[act_y][act_x-1] || lab[act_y][act_x+1]) && (lab[act_y-1][act_x] || lab[act_y+1][act_x]) ||
				(start.x == x && start.y == y) || (end.x == x && end.y == y)){
				if(node != 0){
					if(lab[act_y][act_x-1] && prev_y[y] != -1){
						neigh[node][0] = prev_y[y] - node;
						if(neigh[prev_y[y]][1] == 9999999){
							neigh[prev_y[y]][1] = node - prev_y[y];
						}
					}
					if(lab[act_y-1][act_x] && prev_x[x] != -1){
						neigh[node][2] = prev_x[x] - node;
						if(neigh[prev_x[x]][3] == 9999999){

							neigh[prev_x[x]][3] = node - prev_x[x];
						}
					}
				}

				prev_x[x] = node;
				prev_y[y] = node;
				node++;
			}
		}
	}

	for(int i = 0; i < node; i++){
		printf("%d: ", i);
		for(int j = 0; j < 4; j++){
			printf("%d ", neigh[i][j]);
		}
		printf("\n");
	}

	for(int i = 0; i < N; i++)
		free(neigh[i]);
	free(neigh);
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
