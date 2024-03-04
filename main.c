#include<stdio.h>
#include<stdlib.h>

#include "data.h"
#include "algs.h"

int main(){
	FILE* f = fopen("maze.txt", "r");
	
	// divide into two functions
	point_t* size_n_send = get_lab_info(f);
	
	fclose(f);
	f = fopen("maze.txt", "r");
	
	// create max-dim fining func
	int** lab = zero_vec(size_n_send[0]);
	int** vis = zero_vec(size_n_send[0]);
	
	printf("empty lab[%d, %d]\n", size_n_send[0].y, size_n_send[0].x);

	point_t size = {size_n_send[0].x-1, size_n_send[0].y-1};
	point_t start = {1, 1};
	point_t end = {size.x-1, size.y-1};
	
	box_t* boxes = get_division_points(size_n_send[0]);
	
	for(int i = 0; i < 9; i++){
		printf("(%d, %d, %d, %d)\n", boxes[i].A.x, boxes[i].A.y, boxes[i].B.x, boxes[i].B.y);
	}

	file_to_vec(f, lab, boxes[0].A, boxes[0].B);
	
	lab[1][0] = 0;

	// extract send blocks
	fclose(f);
	f = fopen("maze.txt", "r");
	int box = 0;
	traverse(lab, vis, size, end, boxes, f, &box, start, start);
	fclose(f);	
}
