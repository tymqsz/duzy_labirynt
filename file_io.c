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
		perror("write unsuccesful");
		exit(EXIT_FAILURE);
	}
	fclose(f);
}

int* read_array_binary(char* filename, int offset, int size) {
    FILE* f = fopen(filename, "rb");
	
	//printf("reading %d ints\n", size);
	fseek(f, offset*sizeof(int), SEEK_SET);
    int* arr = malloc(sizeof(int) * (size));
	if(fread(arr, sizeof(int), size, f) != size){
		perror("read unsuccessful");
		fclose(f);
		free(arr);
		exit(EXIT_FAILURE);
	}

    fclose(f);
    return arr;
}

int read_digit_binary(char* filename, int offset){
	FILE* f = fopen(filename, "rb");
	
	fseek(f, offset*sizeof(int), SEEK_SET);
    int digit;
	if(fread(&digit, sizeof(int), 1, f) != 1){
		perror("read unsuccessful");
		fclose(f);
		exit(EXIT_FAILURE);
	}

    fclose(f);
    return digit;
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
