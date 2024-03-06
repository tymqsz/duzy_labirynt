#include <stdio.h>
#include <stdlib.h>

#include "data.h"

void file_to_vec(char* filename, char** v, box_t* box){
		FILE* f = fopen(filename, "r");
		if(f == NULL){
			fprintf(stderr, "cannot open file %s\n", filename);
			exit(1);
		}

		int x = 0, y = 0;
		int c;

		while(x != box->A.x || y != box->A.y){
				c = fgetc(f);
				x++;
				
				if(c == '\n'){
					x = 0;
					y++;
				}
		}
		
		int lab_y = 0;
		while(y < box->B.y){
			while((c = fgetc(f)) != '\n'){
				if(x >= box->A.x && x < box->B.x && y < box->B.y && y >= box->A.y){
					if(c == 'X')
						v[y-box->A.y][x-box->A.x] = 0;
					else
						v[y-box->A.y][x-box->A.x] = 1;
					
					
					if(c == 'P')
						v[y-box->A.y][x-box->A.x+1] = 0;
					if(c == 'K')
						v[y-box->A.y][x-box->A.x-1] = 0;
				}
				x++;
			}
			y++;
			x=0;
		}
		
		fclose(f);
}
	

void change_file_position(char* filename, point_t* lab_size, point_t* position, char value){
	FILE* f = fopen(filename, "rb+");
	if(f == NULL){
		perror("error\n");
		
		exit(1);
	}
	
	int actual_position = position->y*lab_size->x + position->x;
	char offset = 48;
	char actual_value = offset+value;
	

	fseek(f, actual_position, SEEK_SET);

	fwrite(&actual_value, sizeof(char), 1, f);

	fclose(f);
}

char read_file_position(char* filename, point_t* lab_size, point_t* position){
	FILE* f = fopen(filename, "r");
	int actual_position = position->y*lab_size->x + position->x;
	
	fseek(f, actual_position, SEEK_SET);

	char value;
	value = fgetc(f) - 48;	
	
	fclose(f);

	return value;
}

void init_array_binary(char* filename, int size, int val){
	FILE* f = fopen(filename, "wb");
	
	for(int i =  0; i < size; i++)
		fwrite(&val, sizeof(int), 1, f);
	
	fclose(f);
}

void write_array_binary(char* filename, int* arr, int size){
	FILE* f = fopen(filename, "wb");

	if(fwrite(arr, sizeof(int), size, f) == size)
		printf("write succesful\n");
	else{
		perror("write unsuccesful\n");
		exit(EXIT_FAILURE);
	}
	fclose(f);
}

int* read_array_binary(char* filename, int offset, int size) {
    FILE* f = fopen(filename, "rb");
	
	fseek(f, offset*sizeof(int), SEEK_SET);
    int* arr = malloc(sizeof(int) * (size));
	
	if(fread(arr, sizeof(int), size, f) != size){
		perror("read unsuccessful\n");
		fclose(f);
		free(arr);
		exit(EXIT_FAILURE);
	}

    fclose(f);
    return arr;
}

void update_array_binary(char* filename, int offset, int value){
	FILE* f = fopen(filename, "r+b");
	fseek(f, offset*sizeof(int), SEEK_SET);
	
	if(fwrite(&value, sizeof(int), 1, f) != 1){
		printf("update unsuccessful\n");
		fclose(f);
		exit(EXIT_FAILURE);
	}
	fclose(f);
}

void init_visited_file(char* filename, point_t lab_size){
	FILE* f = fopen(filename, "w");
	char zero = 48;
	int actual_size = lab_size.x*lab_size.y;

	for(int i = 0; i < actual_size; i++)
		fprintf(f, "%c", zero);
	
	fprintf(f, "%c", '\n');
	fclose(f);
}

void init_queue(char* filename){
	FILE* f = fopen(filename, "w");
	
	fclose(f);
}

void append(char* filename, point_t value){
	FILE* f = fopen(filename, "a");

	fprintf(f, "%d %d\n", value.x, value.y);

	fclose(f);
}

point_t top(char* filename, int top_line){
	int crt_line = 0;
	FILE* f = fopen(filename, "r");
	
	char buffor[20];
	point_t t;
	int x, y;

	while(fgets(buffor, sizeof(buffor), f) != NULL){
		sscanf(buffor, "%d %d", &(t.x), &(t.y));
		
		if(crt_line == top_line)
			break;
		
		crt_line++;
	}
	
	fclose(f);
	return t;
}
