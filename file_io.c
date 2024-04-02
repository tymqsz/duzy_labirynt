#include <stdio.h>
#include <stdlib.h>

#include "data.h"

/* wczytanie czesci labiryntu z pliku tekstowego
   okreslonej przez blok */
void lab_to_array(char* filename, char** vec, block_t block, point_t size){
		FILE* f = fopen(filename, "r");
		if(f == NULL){
			fprintf(stderr, "cannot open file %s\n", filename);
			exit(EXIT_FAILURE);
		}
		
		int x = 0, y = 0;
		int c;

		/* przejdz do pozycji startowej (lewy gorny rog bloku), +1 bo '\n' */
		fseek(f, block.mini.y*(size.x+1)+block.mini.x, SEEK_SET); 
		x = block.mini.x;
		y = block.mini.y;
		
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
		//skip
		fclose(f);
}

/* funkcja znajdujaca odpowiedni blok w ktorym zawiera sie pole crt
   i wywolujaca funkcje lab_to_array */
void load_proper_block(point_t crt, block_t* blocks, char* filename, char** lab, int *block_index, point_t size){
	/* jesli crt zawiera sie w aktualnie wczytanym bloku, nie rob nic */
	if(crt.x > blocks[*block_index].mini.x && crt.x < blocks[*block_index].maxi.x &&
	   crt.y > blocks[*block_index].mini.y && crt.y < blocks[*block_index].maxi.y)
		return;
	
	for(int i = 0; i < 9; i++){
		if(crt.x > blocks[i].mini.x && crt.x < blocks[i].maxi.x &&
		   crt.y > blocks[i].mini.y && crt.y < blocks[i].maxi.y){
			/* wczytanie blocku i zmiana indeksu bloku */
			lab_to_array(filename, lab, blocks[i], size);
			*block_index = i;
			return;
		}
	}
	
	fprintf(stderr, "Blad wczytywania bloku\n");
	exit(EXIT_FAILURE);
}

/* inicjalizacja wektora przechowywanego w pliku binarnym
   o wielkosci size i wartosciach value */
void init_file_vector(char* filename, int size, int value){
	FILE* f = fopen(filename, "wb");
	if(f == NULL){
		printf("Nie moge otworzyc pliku %s\n", filename);
		exit(EXIT_FAILURE);
	}
	
	for(int i =  0; i < size; i++)
		fwrite(&value, sizeof(int), 1, f);
	
	fclose(f);
}

/* wczytanie do wektora danych z pliku binarnego od indeksu 
   offset w liczbie size */
int* read_file_vector(char* filename, int offset, int size) {
    FILE* f = fopen(filename, "rb");
	if(f == NULL){
		printf("Nie moge otworzyc pliku %s\n", filename);
		exit(EXIT_FAILURE);
	}
	fseek(f, offset*sizeof(int), SEEK_SET);
    
	int* vec = malloc(sizeof(int) * (size));
	if(fread(vec, sizeof(int), size, f) != size){
		printf("Nie moge czytac indeksow %d-%d pliku %s\n", offset, offset+size, filename);
		fclose(f);
		free(vec);
		exit(EXIT_FAILURE);
	}

    fclose(f);
    return vec;
}

/* wczytanie pozycji offset z pliku binarnego */
int read_file_position(char* filename, int offset){
	FILE* f = fopen(filename, "rb");
	if(f == NULL){
		printf("Nie moge otworzyc pliku %s\n", filename);
		exit(EXIT_FAILURE);
	}
	fseek(f, offset*sizeof(int), SEEK_SET);
	
	int value;
	if(fread(&value, sizeof(int), 1, f) != 1){
		printf("Nie moge czytac indeksu %d pliku %s\n", offset, filename);
		fclose(f);
		exit(EXIT_FAILURE);
	}

    fclose(f);
    return value;
}

/* zamiana pozycji offset na wartosc value w pliku binarnym */
void update_file_vector(char* filename, int offset, int value){
	FILE* f = fopen(filename, "r+b");
	if(f == NULL){
		printf("Nie moge otworzyc pliku %s\n", filename);
		exit(EXIT_FAILURE);
	}
	fseek(f, offset*sizeof(int), SEEK_SET);
	
	if(fwrite(&value, sizeof(int), 1, f) != 1){
		printf("Nie moge pisac do indeksu %d pliku %s\n", offset, filename);
		fclose(f);
		exit(EXIT_FAILURE);
	}
	fclose(f);
}

/* usuniecie plikow tymczasowych */
void delete_temp_files(char** temp_files, int n_files){
	for(int i = 0; i < n_files; i++){
		if(remove(temp_files[i]) != 0){
			fprintf(stderr, "Nie moge usunac pliku %s\n", temp_files[i]);
			exit(EXIT_FAILURE);
		}
	}
}
