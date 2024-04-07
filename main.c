#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "data.h"
#include "bfs.h"
#include "file_vector.h"
#include "file_io.h"
#include "queue.h"
#include "metadata.h"

char* TEMP_BIN_FILES[] = {QUEUE_BIN, GRAPH_BIN, PARENT_BIN, PATH_BIN}; /* sciezki do plikow tymczasowych */

int main(int argc, char** argv){
	char* input_filename;
	if(argc > 1)
		input_filename = argv[1];
	else{
		printf("nie podano pliku wejsciowego\n");
		exit(EXIT_FAILURE);
	}
	
	int binary_output = argc > 2 ? atoi(argv[2]): 0;
	int verbose = argc > 3 ? atoi(argv[3]) : 0;
	
	/* obsluga binarnego/tekstowego pliku wejsciowego */
	point_t lab_size, start, end;
	int start_left; /* zmienna przechowujaca info o kierunku wejscia do labiryntu*/
	if(strstr(input_filename, ".bin") != NULL){
		lab_info_binary(input_filename, &lab_size, &start, &end);

		binary_to_txt(input_filename, "lab.txt", lab_size);
		input_filename = "lab.txt";
	}
	else{
		lab_info_txt(input_filename, &lab_size, &start, &end, &start_left);
	}
	int start_node = coords_to_node(start, lab_size);
	int end_node = coords_to_node(end, lab_size);
	point_t true_size = {(lab_size.x-1)/2, (lab_size.y-1)/2};

	if(verbose)
		printf("plik wczytany\n");
	/* utworzenie grafu na podst. labiryntu
	   i zapisanie go w pliku GRAPH_BIN */
	graph_to_bin_file(input_filename, true_size);
	if(verbose)
		printf("graf zapisany w pliku binarnym\n");
	
	/* przejscie po labiryncie i zapisanie sciezki */
	traverse(start_node, end_node, true_size);
	if(verbose)
		printf("sciezka znaleziona\n");


	/* wypisanie rozwiazania */
	if(binary_output){
		compress_lab_to_binary(input_filename, lab_size, start, end);
		path_to_binary(start_node, end_node, true_size);
	}
	else{
		path_to_txt(start_node, end_node, true_size, start_left);
	}


	/* usuniecie plikow tymczasowych */
	delete_temp_files(TEMP_BIN_FILES, 4);
	if(verbose)
		printf("pliki tymczasowe usuniete\n");
	
	return 0;
}