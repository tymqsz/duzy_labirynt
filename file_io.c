#include <stdio.h>
#include <stdlib.h>

#include "data.h"

/* BINARY INPUT */
void get_lab_info(point_t* size) {
    FILE* f = fopen("maze.bin", "rb");
    if (f == NULL) {
        printf("Error opening file.\n");
        return;
    }

    fseek(f, 5, SEEK_SET); // Move to the start of data section

    short int cols, rows, entry_x, entry_y, exit_x, exit_y;
    fread(&cols, sizeof(short int), 1, f); // Use short int for reading 2-byte integers
    fread(&rows, sizeof(short int), 1, f);
    fread(&entry_x, sizeof(short int), 1, f);
    fread(&entry_y, sizeof(short int), 1, f);
    fread(&exit_x, sizeof(short int), 1, f);
    fread(&exit_y, sizeof(short int), 1, f);

	fseek(f, 37, SEEK_SET);

	char sep, w, p;
	fread(&sep, 1, 1, f);
	fread(&w, 1, 1, f);
	fread(&p, 1, 1, f);

	size->x = cols;
	size->y = rows;

    fclose(f);
}

void convert_to_txt(char* input, char* output, point_t lab_size){
	FILE* in = fopen(input, "rb");
	FILE* out = fopen(output, "w");
	
	printf("%d %d \n", lab_size.x, lab_size.y);
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

/* END OF BINARY INPUT */


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
