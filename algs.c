#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "file_io.h"
#include "queue.h"
#include "metadata.h"

/* funkcja wczytujaca odpowiednia czesc wektora parent */
void reload_parent(int**, int*, int*, int, int, int);

/* funkcja wczytujaca odpowiednia czesc wektora graph */
void reload_graph(int**, int*, int*, int, int, int);

/* funkcja wypisujaca znaleziona sciezke na podst. pliku PATH_BIN */
void reconstruct_path(int start, int end, point_t true_size);


/* funkcja przechodzaca po labiryntcie dzialajaca na zasadzie bfs */
void traverse(int start_node, int end_node, point_t true_size){
	int n_nodes = true_size.x*true_size.y;
	int HELD_PARENTS = 60000; /* liczba trzymanych indeksow wektora parent w jednym czasie */
	int HELD_NODES = 60000; /* liczba trzymanych indeksow wektora graph w jednym czasie */
	int min_node = 0, max_node = HELD_NODES; /* przedzial trzymanych indeksow wektora graph */
	int min_parent = 0, max_parent = HELD_PARENTS; /* przedzial trzymanych indeksow wektora parent */
	if(n_nodes < HELD_PARENTS)
		HELD_PARENTS = n_nodes; /* trzymaj maksymalnie wszystkie wierzcholki */
	if(n_nodes < HELD_NODES)
		HELD_NODES = n_nodes; /* trzymaj maksymalnie wszystkie wierzcholki */

	Queue_t* queue = init_queue(INTERNAL_QUEUE_SIZE, EXTERNAL_QUEUE_SIZE); 

	int node = start_node, next;
	int EMPTY = true_size.x*true_size.y + 1; /* int oznaczajacy nieodwiedzony wierzcholek */

	init_file_vector(PARENT_BIN, n_nodes, EMPTY); /* inicjalizacja wektora w pliku z wszystkimi wierzcholkami nieodwiedzonymi */
	update_file_vector(PARENT_BIN, node, node);	 /* oznaczenie pierwszego wierzcholka jako odwiedzonego */
	push(queue, node); /* dodanie pierwszego wierzcholka do kolejki */
	
	int* parent = read_file_vector(PARENT_BIN, node, HELD_PARENTS); /* wczytanie pierwszej czesc wektora parent */
	int* graph = read_file_vector(GRAPH_BIN, 0, 4*HELD_NODES); /* wczytanie pierwszej czesc wektora graph */
	
	while(queue->internal_size > 0){
		node = pop(queue); /* zdjecie pierwszego wierzcholka z kolejki */
		if(node == end_node){
			printf("sciezka znaleziona\n");
			break;
		}

		reload_graph(&graph, &min_node, &max_node, node, n_nodes, HELD_NODES); /* wczytaj odpowiednia czesc graph */
		
		/* sprawdzenie wszystkich sasiadow wierzcholka node,
		   informacje o sasiadach wierzcholka n-tego znajduja sie w wektorze graph
		   pod indeksami (n-min_node)*4+0...(n-min_node)*4+3 */
		for(int i = 0; i < 4; i++){
			/* sprawdzenie czy sasiad != -1 (przejscie istnieje) */
			if(graph[(node-min_node)*4+i] != -1){
				next = graph[(node-min_node)*4+i];
				
				reload_parent(&parent, &min_parent, &max_parent, next, n_nodes, HELD_PARENTS); /* wczytaj odp. czesc parent */
				
				/* sprawdzenie czy sasiad nieodwiedzony */
				if (parent[next-min_parent] == EMPTY){
					update_file_vector(PARENT_BIN, next, node); /* oznaczenie sasiada jako odwiedzonego */
					parent[next-min_parent] = node;
					push(queue, next); /* dodanie sasiada do kolejki */
				}
			}
		}
	}

	free(graph);
	free(parent);

	reconstruct_path(start_node, end_node, true_size);
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

/* funkcja zapisujaca znaleziona scizeke do pliku */
void reconstruct_path(int start, int end, point_t true_size){
	int crt = end;
	int read;
	 
	init_file_vector(PATH_BIN, true_size.x*true_size.y, -1); /* inicjalizacja pliku binarnego
																do przechowywania sciazki */
	/* przepisanie sciezki do pliku binarnego */
	int node = 0;
	while(crt != start){
		read = read_file_position(PARENT_BIN, crt);
		 
		update_file_vector(PATH_BIN, node, crt);
	
		crt = read;
		node++;
	}
	update_file_vector(PATH_BIN, node, crt);
	
	
	int x, y, prev_x, prev_y, steps = 0;
	point_t dir;
	int dir_index, prev_dir_index = -1;
	char turn[20];
	FILE* f = fopen("path.txt", "w");
	
	/* zapisanie sciezki w postaci krokow do pliku path.txt */
	crt = read_file_position(PATH_BIN, node);
	prev_x = crt % true_size.x;
	prev_y = crt / true_size.x;
	node--;
	while(node >= 0){
		crt = read_file_position(PATH_BIN, node);
		
		/* obliczenie koordynatow danego wierzcholka */
		x = crt % true_size.x; 
		y = crt / true_size.x;
		
		/* obliczenie wektora przesuniecia na podst.
		   pozycji aktualnej i poprzedniej */
		dir.x = x - prev_x;
		dir.y = y - prev_y;
		
		dir_index = get_dir_index(dir); /* sprawdzenie indeksu wektora przesuniecia */
		
		/* jesli kierunek ten sam co poprzednio, zwiekszenie liczby krokow */
		if(prev_dir_index == -1 || prev_dir_index == dir_index)
			steps++;
		else{
			/* w przeciwnym wypadku sprawdzenie kierunku skretu i wypisanie liczby krokow */
			if(dir_index - prev_dir_index == 1 || dir_index - prev_dir_index == -3)
				strcpy(turn, "TURNRIGHT");
			else
				strcpy(turn, "TURNLEFT");
			
			fprintf(f, "FORWARD %d\n%s\n", steps, turn);
			steps = 1;
		}

		prev_dir_index = dir_index;
		prev_x = x;
		prev_y = y;

		node--;
	}
	/* wypisanie ostatniej prostej */
	fprintf(f, "FORWARD %d\n", steps);
	fclose(f);
}

void reload_parent(int** parent, int* min_parent, int* max_parent, int crt, int n_nodes, int HELD_PARENTS){
		if(crt < *min_parent || crt >= *max_parent){
			*min_parent = max(0, crt - HELD_PARENTS/2);
			*max_parent = min(n_nodes, *min_parent+HELD_PARENTS);
			
			free(*parent);
			
			*parent = read_file_vector(PARENT_BIN, *min_parent, *max_parent-*min_parent);
		}
}

void reload_graph(int** graph, int* min_node, int* max_node, int crt, int n_nodes, int HELD_NODES){
		if(crt < *min_node || crt >= *max_node){
			*min_node = max(0, crt-HELD_NODES/2);
			*max_node = min(*min_node+HELD_NODES, n_nodes);

			free(*graph);

			*graph = read_file_vector(GRAPH_BIN, 4*(*min_node), (*max_node-*min_node)*4);
		}
}
