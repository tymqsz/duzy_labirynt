#include <stdio.h>
#include <stdlib.h>

#include "data.h"

void lab_to_vec(char* filename, char** vec, block_t block){
		FILE* f = fopen(filename, "r");
		if(f == NULL){
			fprintf(stderr, "cannot open file %s\n", filename);
			exit(EXIT_FAILURE);
		}

		int x = 0, y = 0;
		int c;
		while(x != block.mini.x || y != block.mini.y){
				c = fgetc(f);
				x++;
				
				if(c == '\n'){
					x = 0;
					y++;
				}
		}
		
		while(y < block.maxi.y){
			while((c = fgetc(f)) != '\n'){
				if(x >= block.mini.x && x < block.maxi.x && y < block.maxi.y && y >= block.mini.y){
					if(c == 'X' || c == 'P' || c == 'K')
						vec[y - block.mini.y][x - block.mini.x] = 0;
					else
						vec[y - block.mini.y][x - block.mini.x] = 1;
				}
				x++;
			}
			y++;
			x=0;
		}

		fclose(f);
}
	
void init_file_vector(char* filename, int size, int value){
	FILE* f = fopen(filename, "wb");
	
	for(int i =  0; i < size; i++)
		fwrite(&value, sizeof(int), 1, f);
	
	fclose(f);
}

int* read_file_vector(char* filename, int offset, int size) {
    FILE* f = fopen(filename, "rb");
	fseek(f, offset*sizeof(int), SEEK_SET);
    
	int* vec = malloc(sizeof(int) * (size));
	if(fread(vec, sizeof(int), size, f) != size){
		perror("read unsuccessful");
		fclose(f);
		free(vec);
		exit(EXIT_FAILURE);
	}

    fclose(f);
    return vec;
}

int read_file_position(char* filename, int offset){
	FILE* f = fopen(filename, "rb");
	fseek(f, offset*sizeof(int), SEEK_SET);
	
	int value;
	if(fread(&value, sizeof(int), 1, f) != 1){
		perror("read unsuccessful");
		fclose(f);
		exit(EXIT_FAILURE);
	}

    fclose(f);
    return value;
}

void update_file_vector(char* filename, int offset, int value){
	FILE* f = fopen(filename, "r+b");
	fseek(f, offset*sizeof(int), SEEK_SET);
	
	if(fwrite(&value, sizeof(int), 1, f) != 1){
		printf("update unsuccessful\n");
		fclose(f);
		exit(EXIT_FAILURE);
	}
	fclose(f);
}

void delete_temp_files(char** temp_files, int n_files){
	for(int i = 0; i < n_files; i++){
		if(remove(temp_files[i]) != 0){
			fprintf(stderr, "couldnt remove %s\n", temp_files[i]);
			exit(EXIT_FAILURE);
		}
	}
}
