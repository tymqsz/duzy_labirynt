#include<stdlib.h>
#include<stdio.h>

#include "data.h"
#include "file_vector.h"
#include "file_io.h"
#include "metadata.h"

void binary_to_txt(char* input, char* output, point_t lab_size){
	FILE* in = fopen(input, "rb");
	FILE* out = fopen(output, "w");
	
	fseek(in, 40, SEEK_SET);

	int count = 0;
	int target = lab_size.x*lab_size.y;
	

	unsigned char g, val;
	unsigned char cnt;
	while(count < target){
		fread(&g, 1, 1, in);
		fread(&val, 1, 1, in);
		fread(&cnt, 1, 1, in);
		
		for(int i = 0; i < cnt+1; i++){
			if((count+i) % lab_size.x == 0 && count+i != 0){
				fprintf(out, "\n");
			}
			fprintf(out, "%c", val);
		}
		count += (cnt+1);
	}
	
	fclose(in);
	fclose(out);
}

void graph_to_bin_file(char* input_file, point_t size){
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

int coords_to_node(point_t coords, point_t lab_size){
	if(coords.x ==  0)
		coords.x++;
	else if(coords.y == 0)
		coords.y++;
	else if(coords.x == lab_size.x-1)
		coords.x--;
	else if(coords.y == lab_size.y-1)
		coords.y--;

	int node;

	node = (coords.x-1)/2 + (coords.y-1)/2*(lab_size.x-1)/2;

	return node;
}

/* funkcja przyporzadkowujaca ideks do wektora kierunku:
   wektor [-1, 0] (lewo) -> indeks 0,
   wektor [0, -1] (gora) -> indeks 1,
   ...
*/
int get_dir_index(point_t dir){
	if(dir.x == -1)
		return 0;
	if(dir.x == 1)
		return 2;
	if(dir.y == -1)
		return 1;
	if(dir.y == 1)
		return 3;
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
