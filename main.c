#include<stdio.h>
#include<stdlib.h>
#define MAX_NODES 1000
#include "data.h"
#include "algs.h"
#include "file_io.h"

char INPUT[] = "maze.txt";
char VISITED[] = "visited.txt";

int main(){
	// divide into two functions
	point_t* size_n_send = get_lab_info(INPUT);
	
	box_t* boxes = get_division_points(size_n_send[0]);
	point_t buffor_size = biggest_box(boxes);


	char** lab = zero_vec(buffor_size);
	
	point_t size = {size_n_send[0].x, size_n_send[0].y};
	point_t start = {1, 1};
	point_t end = {size.x-2, size.y-2};
	
	int box = 0;

	file_to_vec(INPUT, lab, &boxes[0]);
	
	point_t true_size = {(size.x-1)/2, (size.y-1)/2};
	extract_nodes(lab, true_size, start, end, boxes, INPUT, box);
	
	int* nodes = read_array_binary("neigh.bin", true_size.x*true_size.y*4-1, 1);
	printf("no. of nodes: %d\n", *nodes);
	
	free_vec(lab, buffor_size);
}
