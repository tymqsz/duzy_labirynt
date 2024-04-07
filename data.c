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
	

	char g, val;
	unsigned char cnt;
	while(count < target){
		fread(&g, 1, 1, in);
		fread(&val, 1, 1, in);
		fread(&cnt, 1, 1, in);
		
		count += (cnt+1);

		for(int i = 0; i < cnt+1; i++)
			fprintf(out, "%c", val);
		if( count % lab_size.x == 0)
			fprintf(out, "\n");
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

/* funkcja znajdujaca liczbe kolumn i wierszy
   ktore reprezentuja labirynt w pliku .txt */
void lab_info_txt(char* filename, point_t* lab_size, point_t* start, point_t* end, int* start_left){
	FILE* f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "Nie moge czytac pliku %s\n", filename);
		exit(1);
	}
	
	/* znalezienie rozmiaru pliku */
	int c;
	int x = 0, y = 0;
	int row_len_found = 0;
	while((c = fgetc(f)) != EOF){
		if(c == '\n' && !row_len_found){
			lab_size->x = x;
			row_len_found = 1;
		}
		if(c == '\n'){
			y += 1;
		}
		x += 1;
	}
	lab_size->y = y+1;
	
	/* znalezienie wejscia i wyjscia */
	x = 0, y = 0;
	fseek(f, 0, SEEK_SET);
	while((c = fgetc(f)) != EOF){
		if((x == 0 || y == 0) && c == 'P'){
			if(x==0)
				*start_left = 1;
			else
				*start_left = 0;
			start->x = x;
			start->y = y;
		}
		if((x == lab_size->x-1 || y == lab_size->y-1) && c == 'K'){
			end->x = x;
			end->y = y;
		}

		x += 1;
		if(c == '\n'){
			y += 1;
			x = 0;
		}
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

void lab_info_binary(char* filename, point_t* size, point_t* start, point_t* end) {
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Nie moge czytac pliku %s\n", filename);
        exit(1);
    }

    fseek(f, 5, SEEK_SET);

    short int cols, rows, entry_x, entry_y, exit_x, exit_y;
    fread(&cols, sizeof(short int), 1, f); 
    fread(&rows, sizeof(short int), 1, f);
    fread(&entry_x, sizeof(short int), 1, f);
    fread(&entry_y, sizeof(short int), 1, f);
    fread(&exit_x, sizeof(short int), 1, f);
    fread(&exit_y, sizeof(short int), 1, f);


	size->x = cols;
	size->y = rows;
	
	start->x = entry_x-1;
	start->y = entry_y-1;

	end->x = exit_x-1;
	end->y = exit_y-1;

    fclose(f);
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
