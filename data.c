#include<stdlib.h>
#include<stdio.h>

#include "data.h"

char** zero_vec(point_t size){
	char** new = calloc(size.y, sizeof(char*));
	for (int i = 0; i < size.y; i++)
		new[i] = calloc(size.x, sizeof(char));

	return new;
}

void free_vec(char** vec, point_t size){
	for(int i = 0; i < size.y; i++){
		free(vec[i]);
	}
	free(vec);
}

point_t* get_lab_info(char* filename){
	FILE* f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "cannot open file %s\n", filename);
		exit(1);
	}

	point_t* coords = malloc(sizeof(point_t)*3);

	int c;
	int x = 0, y = 0;
	int row_len_found = 0;

	while((c = fgetc(f)) != EOF){
		if(c == 'P'){
			coords[1].x = x;
			coords[1].y = y;
		}
		if(c == 'K'){
			coords[2].x = x;
			coords[2].y = y;
		}
		if(c == '\n' && !row_len_found){
			coords[0].x = x;
			row_len_found = 1;
		}
		else if(c == '\n'){
			y += 1;
		}
		x += 1;
	}
	coords[0].y = y+1;
	
	fclose(f);
	return coords;
}

box_t new_box(int x1, int y1, int x2, int y2){
	point_t A, B;
	A.x = x1;
	A.y = y1;
	B.x = x2;
	B.y = y2;

	box_t new;	
	new.A = A;
	new.B = B;

	return new;
}


box_t* get_division_points(point_t size){
	box_t* boxes = malloc(sizeof(box_t)*9);
	
	int x1, x2, x3;
	int y1, y2, y3;

	x1 = size.x/3;
	x2 = 2*(size.x/3)-1;
	x3 = size.x;
	
	y1 = size.y/3;
	y2 = 2*(size.y/3)-1;
	y3 = size.y;
	
	if(x1 % 2 == 0)
		x1 += 1;
	if(x2 % 2 == 0)
		x2 += 1;
	if(x3 % 2 == 0)
		x3 += 1;
	if(y1 % 2 == 0)
		y1 += 1;
	if(y2 % 2 == 0)
		y2 += 1;
	if(y3 % 2 == 0)
		y3 += 1;
	
	boxes[0] = new_box(0, 0, x1, y1);
	boxes[1] = new_box(x1-1, 0, x2, y1);
	boxes[2] = new_box(x2-3, 0, x3, y1);

	boxes[3] = new_box(0, y1-1, x1, y2);
	boxes[4] = new_box(x1-1, y1-1, x2,y2);
	boxes[5] = new_box(x2-3, y1-1, x3, y2);

	boxes[6] = new_box(0, y2-3, x1, y3);
	boxes[7] = new_box(x1-1, y2-3, x2, y3);
	boxes[8] = new_box(x2-3, y2-3, x3, y3);
	
	return boxes;
}

point_t biggest_box(box_t* boxes){
	point_t size = {0, 0};

	for(int i = 0; i < 9; i++){
		if(boxes[i].B.x-boxes[i].A.x > size.x)
			size.x = boxes[i].B.x-boxes[i].A.x;
		if(boxes[i].B.y-boxes[i].A.y > size.y)
			size.y = boxes[i].B.y-boxes[i].A.y;
	}

	return size;
}

int max(int a, int b){
	if(a > b)
		return a;
	return b;
}

int min(int a, int b){
	if(a < b)
		return a;
	return b;
}
