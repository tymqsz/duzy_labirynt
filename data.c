#include<stdlib.h>
#include<stdio.h>

#include "data.h"
#include "file_io.h"
#include "metadata.h"

/* inicjalizacja tablicy 2d */
char** zero_array(point_t size){
	char** new = calloc(size.y, sizeof(char*));
	for (int i = 0; i < size.y; i++)
		new[i] = calloc(size.x, sizeof(char));

	return new;
}

/* zwolnienie pamieci po tablicy 2d */
void free_array(char** array, point_t size){
	for(int i = 0; i < size.y; i++){
		free(array[i]);
	}
	free(array);
}

void lab_to_bin_file(char* input_file, point_t size){
	int n_nodes = size.x*size.y;
	init_file_vector(GRAPH_BIN, 4*n_nodes, -1);
	
	point_t lab_size = {size.x*2+1, size.y*2+1};
	
	FILE* f = fopen(input_file, "r");
	
	int i = 0;
	point_t crt;
	char c;
	int node;
	while(i < lab_size.x*lab_size.y){
		crt.y = i / lab_size.x;
		crt.x = i % lab_size.x;
		

		c = fgetc(f);
		if(c == '\n')
			continue;
		
		if(crt.y % 2 == 1 && crt.x % 2 == 0 && c == ' '){
			if (crt.x > 0){
				node = size.x*(crt.y-1)/2 + (crt.x-2)/2;
				update_file_vector(GRAPH_BIN, node*4+1, node+1);
			}
			if (crt.x < lab_size.x){
				node = size.x*(crt.y-1)/2 + (crt.x)/2;
				update_file_vector(GRAPH_BIN, node*4, node-1);
			}
		}
		if(crt.y % 2 == 0 && crt.x % 2 == 1 && c == ' '){
			if (crt.y > 0){
				node = size.x*(crt.y-2)/2 + (crt.x-1)/2;
				update_file_vector(GRAPH_BIN, node*4+3, node+size.x);
			}
			if (crt.y < lab_size.y){
				node = size.x*(crt.y)/2 + (crt.x-1)/2;
				update_file_vector(GRAPH_BIN, node*4+2, node-size.x);
			}
		}
		i++;
	}
	fclose(f);
}

/* funkcja znajdujaca liczbe kolumn i wierszy
   ktore reprezentuja labirynt w pliku .txt */
point_t get_lab_size(char* filename){
	FILE* f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "nie ma pliku o takiej nazwie %s\n", filename);
		exit(1);
	}
	
	point_t size;
	int c;
	int x = 0, y = 0;
	int row_len_found = 0;
	while((c = fgetc(f)) != EOF){
		if(c == '\n' && !row_len_found){
			size.x = x;
			row_len_found = 1;
		}
		else if(c == '\n'){
			y += 1;
		}
		x += 1;
	}
	size.y = y+1;
	
	fclose(f);
	return size;
}

/* inicjalizacja bloku (prostokata) opisanego przez dwa punkty:
   A - lewy gorny rog, B - prawy dolny rog */
block_t new_block(int x1, int y1, int x2, int y2){
	point_t A = {x1, y1}, B = {x2, y2};

	block_t new;	
	new.mini = A;
	new.maxi = B;

	return new;
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
