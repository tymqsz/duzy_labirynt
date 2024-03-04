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
