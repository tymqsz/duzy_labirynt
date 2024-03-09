#include<stdio.h>
#include<stdlib.h>

#include "metadata.h"
#include "data.h"
#include "algs.h"
#include "file_io.h"

char* TEMP_FILES[] = {QUEUE_BIN, GRAPH_BIN, PARENT_BIN, NODE_COORDS_BIN};
int n_temp = 4;

int main(){
	char* input_filename = "maze.txt";

	point_t lab_size = get_lab_size(input_filename);	
	block_t* blocks = divide_into_9_blocks(lab_size);
	point_t buffor_size = biggest_block(blocks);
	

	char** lab = zero_vec(buffor_size);
	point_t start = {1, 1};
	point_t end = {lab_size.x-2, lab_size.y-2};
	point_t true_size = {(lab_size.x-1)/2, (lab_size.y-1)/2};

	lab_to_vec(input_filename, lab, blocks[0]);
	

	int block_index = 0;
	extract_nodes(lab, true_size, start, end, blocks, input_filename, block_index);
	free_vec(lab, buffor_size);
	
	
	int n_nodes = read_file_position(GRAPH_BIN, true_size.x*true_size.y*4-1);
	
	bfs(0, n_nodes-1, n_nodes, true_size);

	delete_temp_files(TEMP_FILES, n_temp);
}
