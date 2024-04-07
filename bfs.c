#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "file_vector.h"
#include "file_io.h"
#include "queue.h"
#include "metadata.h"

/* funkcja wczytujaca odpowiednia czesc wektora parent */
void reload_parent(int**, int*, int*, int, int, int);

/* funkcja wczytujaca odpowiednia czesc wektora graph */
void reload_graph(int**, int*, int*, int, int, int);

/* funkcja przechodzaca po labiryntcie dzialajaca na zasadzie bfs */
int traverse(int start_node, int end_node, point_t true_size){
	int n_nodes = true_size.x*true_size.y;
	int HELD_PARENTS = 60000; /* liczba trzymanych indeksow wektora parent w jednym czasie */
	int HELD_NODES = 60000; /* liczba trzymanych indeksow wektora graph w jednym czasie */
	if(n_nodes < start_node+HELD_PARENTS)
		HELD_PARENTS = n_nodes-start_node; /* trzymaj maksymalnie wierzcholki od poczatkowego do ostatniego */
	if(n_nodes < start_node+HELD_NODES)
		HELD_NODES = n_nodes-start_node; /* trzymaj maksymalnie wierzcholki od poczatkowego do ostatniego */
	int min_node = start_node, max_node = HELD_NODES; /* przedzial trzymanych indeksow wektora graph */
	int min_parent = start_node, max_parent = HELD_PARENTS; /* przedzial trzymanych indeksow wektora parent */
	
	Queue_t* queue = init_queue(INTERNAL_QUEUE_SIZE, EXTERNAL_QUEUE_SIZE); 

	int node = start_node, next;
	int EMPTY = true_size.x*true_size.y + 1; /* int oznaczajacy nieodwiedzony wierzcholek */

	init_file_vector(PARENT_BIN, n_nodes, EMPTY); /* inicjalizacja wektora w pliku z wszystkimi wierzcholkami nieodwiedzonymi */
	update_file_vector(PARENT_BIN, node, node);	 /* oznaczenie pierwszego wierzcholka jako odwiedzonego */
	push(queue, node); /* dodanie pierwszego wierzcholka do kolejki */
	
	int* parent = read_file_vector(PARENT_BIN, node, HELD_PARENTS); /* wczytanie pierwszej czesc wektora parent */
	int* graph = read_file_vector(GRAPH_BIN, node*4, 4*HELD_NODES); /* wczytanie pierwszej czesc wektora graph */
	while(queue->internal_size > 0){
		node = pop(queue); /* zdjecie pierwszego wierzcholka z kolejki */

		if(node == end_node){ /*sciezka znaleziona*/
			free(graph);
			free(parent);
			return 0;
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

	return 1; /*brak sciezki*/
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
