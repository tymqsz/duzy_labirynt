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

/* funkcja przepisujaca labirynt z pliku tekstowego do binarnego */
void lab_to_bin_file(char* input_file, point_t true_size, block_t* blocks, point_t buffor_size){
	int n_nodes = true_size.x*true_size.y;
	init_file_vector(GRAPH_BIN, 4*n_nodes, -1); /* inicjalizacja wektora przechowywanego w pliku
												   o rozmiarze 4*liczba wierzcholkow (po 4 sasiadow) */
	
	point_t size = {true_size.x*2+1, true_size.y*2+1}; 
	
	/* inicjalizacja tablicy do przechowywania czesci labiryntu,
	   wczytanie pierwszego bloku */
	char** lab = zero_array(buffor_size);
	int block = 0;
	lab_to_array(input_file, lab, blocks[0], size);
	
	/* przejscie po calym labiryncie i zapisanie informacji
	   o sasiadach wierzcholkow w pliku binarnym*/
	point_t crt;
	for(int node = 0; node < n_nodes; node++){
		/* obliczenie pozycji danego wierzcholka
		   w tekstowym pliku wejsciowym */
		crt.y = 2*(node/true_size.x)+1;
		crt.x = 2*(node%true_size.x)+1;
		
		/* doczytanie odpowiedniej czesci labiryntu */
		load_proper_block(crt, blocks, input_file, lab, &block, size); 
		
		/* zapisanie informacji o przejsciach do sasiednich wierzcholkow */
		if(crt.x > 2 && lab[crt.y - blocks[block].mini.y][crt.x - blocks[block].mini.x -1])
			update_file_vector(GRAPH_BIN, node*4, node-1);
		if(crt.x + 2 < size.x && lab[crt.y - blocks[block].mini.y][crt.x - blocks[block].mini.x +1])
			update_file_vector(GRAPH_BIN, node*4+1, node+1);
		if(crt.y > 2 && lab[crt.y - blocks[block].mini.y-1][crt.x - blocks[block].mini.x])
			update_file_vector(GRAPH_BIN, node*4+2, node-true_size.x);
		if(crt.y + 2 < size.y && lab[crt.y - blocks[block].mini.y+1][crt.x - blocks[block].mini.x])
			update_file_vector(GRAPH_BIN, node*4+3, node+true_size.x);
	}

	free_array(lab, buffor_size);
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

/* podzielenie labiryntu na 9 czesci tak, by krawedzie kazdego bloku
   zawieraly informacje o przejsciach do sasiednich blokow */
block_t* divide_into_9_blocks(point_t size){
	block_t* blocks = malloc(sizeof(block_t)*9);
	
	int x1, x2, x3;
	int y1, y2, y3;

	x1 = size.x/3;
	x2 = 2*(size.x/3)-1;
	x3 = size.x;
	
	y1 = size.y/3;
	y2 = 2*(size.y/3)-1;
	y3 = size.y;
	
	if(x1 % 2 == 0)
		x1 += 1;
	if(x2 % 2 == 0)
		x2 += 1;
	if(x3 % 2 == 0)
		x3 += 1;
	if(y1 % 2 == 0)
		y1 += 1;
	if(y2 % 2 == 0)
		y2 += 1;
	if(y3 % 2 == 0)
		y3 += 1;
	
	blocks[0] = new_block(0, 0, x1, y1);
	blocks[1] = new_block(x1-1, 0, x2, y1);
	blocks[2] = new_block(x2-3, 0, x3, y1);

	blocks[3] = new_block(0, y1-1, x1, y2);
	blocks[4] = new_block(x1-1, y1-1, x2,y2);
	blocks[5] = new_block(x2-3, y1-1, x3, y2);

	blocks[6] = new_block(0, y2-3, x1, y3);
	blocks[7] = new_block(x1-1, y2-3, x2, y3);
	blocks[8] = new_block(x2-3, y2-3, x3, y3);
	
	return blocks;
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

/* funkcja zwracajaca rozmiar najwiekszego
   z podanych blokow */
point_t biggest_block(block_t* blocks){
	point_t size = {0, 0};

	for(int i = 0; i < 9; i++){
		size.x = max(blocks[i].maxi.x-blocks[i].mini.x, size.x);
		size.y = max(blocks[i].maxi.y-blocks[i].mini.y, size.y);
	}

	return size;
}
