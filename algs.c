#include <stdlib.h>
#include <stdio.h>

#include "metadata.h"
#include "data.h"
#include "file_io.h"
#include "queue.h"

void reconstruct_path(int start, int end);

/* funkcja wczytujaca odpowiednia czesc labiryntu na podstawie
   aktualnej pozycji opisanej zmienna crt */
void load_proper_block(point_t* crt, block_t* blocks, char* filename, char** lab, int *block_index){
	/* jesli crt zawiera sie w aktualnie wczytanym bloku, nie rob nic */
	if(crt->x > blocks[*block_index].mini.x && crt->x < blocks[*block_index].maxi.x &&
	   crt->y > blocks[*block_index].mini.y && crt->y < blocks[*block_index].maxi.y)
		return;
	
	for(int i = 0; i < 9; i++){
		if(crt->x > blocks[i].mini.x && crt->x < blocks[i].maxi.x &&
		   crt->y > blocks[i].mini.y && crt->y < blocks[i].maxi.y){
			/* wczytanie blocku i zmiana indeksu bloku */
			lab_to_vec(filename, lab, blocks[i]);
			*block_index = i;
			return;
		}
	}
	
	fprintf(stderr, "Blad wczytywania bloku\n");
	exit(EXIT_FAILURE);
}

/* funkcja zapamietujaca informacje o znaczacych wierzcholkach do pliku GRAPH_BIN */
void extract_nodes(char** lab, point_t size, point_t start, point_t end, block_t* blocks, char* filename, int block_index){
	/* wektory zawierajace numer poprzedniego znaczacego wierzcholka
	   odpowiednio w kolumnie x i rzedzie y */
	int* prev_x = malloc(sizeof(int) * (size.x*2+1));
	int* prev_y = malloc(sizeof(int) * (size.y*2+1));
	for(int i = 0; i < size.x; i++)
		prev_x[i] = -1;
	for(int i = 0; i < size.y; i++)
		prev_y[i] = -1;
	
	/* inicjalizacja wektorow przechowywanych w GRAPH_BIN
	   i NODE_COORDS_BIN 
	   EMPTY: liczba wieksza niz maksymalna liczba wierzchlokow
	   		  (oznacza wierzcholek nieodwiedzony) */ 
	int EMPTY = size.x*size.y+1;
	init_file_vector(GRAPH_BIN, size.x*size.y*4, EMPTY);
	
	init_file_vector(NODE_COORDS_BIN, size.x*size.y*2, -1);
	update_file_vector(NODE_COORDS_BIN, 0, 1);


	point_t crt = {1, 1}; /* wspolrzedne aktualnego pola */
	point_t crt_index = {1, 1}; /* indeksy (x, y) tablicy lab, pod ktorymi trzymane sa
								   pola o aktualnych wsporzednych */
	int node_nr = 0;
	int neigh_left, neigh_down;
	
	/* przejscie po polach labiryntu (labirynt rozmiaru NxN reprezentowany jest
	 								  przez ciag znakow rozmiaru [2N+1]x[2N+1]) */
	while(crt.y < size.y*2+1){
		while(crt.x < size.x*2+1){
			load_proper_block(&crt, blocks, filename, lab, &block_index);
			
			/* przesuniecie crt o wartosci minimalne dla 
			   aktualnie trzymanej czesci labiryntu */
			crt_index.x = crt.x - blocks[block_index].mini.x; 
			crt_index.y = crt.y - blocks[block_index].mini.y;
			
			/* sprawdzenie czy aktualny wierzcholek jest trywialny 
			   i mozna go pominac */
			if(!((lab[crt_index.y][crt_index.x-1] && lab[crt_index.y][crt_index.x+1] && !lab[crt_index.y-1][crt_index.x] && !lab[crt_index.y+1][crt_index.x]) ||
				(!lab[crt_index.y][crt_index.x-1] && !lab[crt_index.y][crt_index.x+1] && lab[crt_index.y-1][crt_index.x] && lab[crt_index.y+1][crt_index.x])) ||
				(start.x == crt.x && start.y == crt.y) || (end.x == crt.x && end.y == crt.y)){
				if(node_nr != 0){
					/* polaczenie aktualnego wierzcholka z poprzednim w znajdujacym
					   sie w tym samym rzedzie*/
					if(lab[crt_index.y][crt_index.x-1] && prev_y[crt.y] != -1){
						update_file_vector(GRAPH_BIN, node_nr*4, prev_y[crt.y]-node_nr);
						neigh_left = read_file_position(GRAPH_BIN, prev_y[crt.y]*4+1); 
						
						if(neigh_left == EMPTY){
							update_file_vector(GRAPH_BIN, prev_y[crt.y]*4+1, node_nr-prev_y[crt.y]);
						}
					}
					/* polaczenie aktualnego wierzcholka z poprzednim w znajdujacym
					   sie w tej samej kolumnie*/
					if(lab[crt_index.y-1][crt_index.x] && prev_x[crt.x] != -1){
						update_file_vector(GRAPH_BIN, node_nr*4+2, prev_x[crt.x]-node_nr);
						neigh_down = read_file_position(GRAPH_BIN, prev_x[crt.x]*4+3); 
						
						if(neigh_down == EMPTY){
							update_file_vector(GRAPH_BIN, prev_x[crt.x]*4+3, node_nr-prev_x[crt.x]);
						}
					}
				}
				
				/* zapamietanie informacji o koordynatach dla wierzcholka
				   o numerze node_nr */
				update_file_vector(NODE_COORDS_BIN, node_nr*2, crt.x);
				update_file_vector(NODE_COORDS_BIN, node_nr*2+1, crt.y);
				
				prev_x[crt.x] = node_nr;
				prev_y[crt.y] = node_nr;
				node_nr++;
			}
			crt.x += 2;
		}
		crt.x = 1;
		crt.y += 2;
	}
	
	free(prev_x);
	free(prev_y);

	/* zapisanie informacji o liczbie znaczacych wierzcholkow */
	update_file_vector(GRAPH_BIN, size.x*size.y*4-1, node_nr);
}

/* syf ktory dzieki informacja o rodzicach wierzcholkow
   znajduje sciezke i wypisuje ja do plku "path.txt */
void reconstruct_path(int start, int end){
	int crt = end;
	int read;
	int x, y;
	int* path = malloc(2*end*sizeof(int));
	for(int k = 0; k < 2*end; k++)
		path[k] = -1;
	
	int i = 0;
	while(crt != start){
		x = read_file_position(NODE_COORDS_BIN, crt*2);
		y = read_file_position(NODE_COORDS_BIN, crt*2+1);

		path[i*2] = x;
		path[i*2+1] = y;
		i++;

		read = read_file_position(PARENT_BIN, crt);

		crt = read;
	}
	x = read_file_position(NODE_COORDS_BIN, start*2);
	y = read_file_position(NODE_COORDS_BIN, start*2+1);
	
	path[i*2] = x;
	path[i*2+1] = y;
	
	FILE* out = fopen("path.txt", "w");
	int l = end-1;
	while(path[l*2] == -1)
		l--;
	while(l >= 0){
		x = path[l*2];
		y = path[l*2+1];
		fprintf(out, "(%d, %d)\n", x, y);
		l--;
	}
	free(path);
}

/* algorytm przechodzacy po grafie i zapamietujacy
   poprzednikow danego wierzcholka */
void bfs(int start, int end, int n_nodes, point_t size){
	Queue_t* queue = init_queue(INTERNAL_QUEUE_SIZE, EXTERNAL_QUEUE_SIZE);
	
	int HELD_NODES = MAX_N_HELD_NODES;
	if(HELD_NODES > n_nodes)
		HELD_NODES = n_nodes;
	
	/* wczytanie czesci grafu zawierajacego informacje
	   o liczbie wierzcholkow HELD_NODES */
	int* nodes = read_file_vector(GRAPH_BIN, 0, 4*HELD_NODES);
	int min_node = 0, max_node = HELD_NODES;
	
	init_file_vector(PARENT_BIN, n_nodes, -1);
	
	int crt = start;
	int vis, next;
	push(queue, crt);
	update_file_vector(PARENT_BIN, crt, crt);
	int EMPTY = size.x*size.y + 1;

	while(queue->internal_size != 0 || queue->external_size != 0){
		crt = pop(queue);
		if(crt == end){
			printf("found a way out\n");
		}

		/* doczytanie odpowiedniego fragmentu grafu,
		   proba wczytania liczby HELD_NODES wierzcholkow,
		   po srodku wektora ma znajdywac sie wierzcholek crt*/
		if(crt >= max_node){
			max_node = min(crt+HELD_NODES/2, n_nodes);
			min_node = max(max_node-HELD_NODES, 0);
			
			free(nodes);
			nodes = read_file_vector(GRAPH_BIN, 4*min_node, 4*(max_node-min_node));
		}
		if(crt < min_node){
			min_node = max(crt-HELD_NODES/2, 0);
			max_node = min(min_node+HELD_NODES, n_nodes);

			free(nodes);
			nodes = read_file_vector(GRAPH_BIN, 4*min_node, 4*(max_node-min_node));
		}
		
		for(int i = 0; i < 4; i++){
			next = crt + nodes[(crt-min_node)*4+i]; /* informacje o sasiadach wierzcholka crt przechowywane
													   sa pod indeksami (crt-min_node)*4, ... , (crt-min_node)*4 + 3 */
			
			/* jesli przejscie istnieje i wierzcholek nieodwiedzony
			   - dodaj go do kolejki i zapamietaj jego rodzica */
			if(next-crt != EMPTY){
				vis = read_file_position(PARENT_BIN, next);
				if(vis == -1){
					update_file_vector(PARENT_BIN, next, crt);
					push(queue, next);
				}
			}
		}
	}
		
	reconstruct_path(0, n_nodes-1);		
}
