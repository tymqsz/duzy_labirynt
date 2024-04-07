#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "data.h"
#include "bfs.h"
#include "file_vector.h"
#include "file_io.h"
#include "queue.h"
#include "metadata.h"

char* TEMP_BIN_FILES[] = {QUEUE_BIN, GRAPH_BIN, PARENT_BIN, PATH_BIN}; /* sciezki do plikow tymczasowych */

int main(int argc, char** argv){
	/* upewnienie sie ze foldery input/output istnieja */
    DIR *input_dir = opendir("input");
	DIR *output_dir = opendir("output");
    if(!input_dir){
		printf("folder \"input\" nie istnieje\n");
		return 13;
	}
	if(!output_dir)
		mkdir("output", 0777);

	/* wczytanie nazw pliku wejsciowego i wyjsciowego*/
	char* input_filename;
	if(argc > 1)
		input_filename = argv[1];
	else{
		printf("nie podano pliku wejsciowego\n");
		return 17;
	}

	char input[30];
	strcpy(input, "input/");
	strcat(input, input_filename);

	char output_filename[30];
	int binary_output = 0;
	if(argc > 2){
		strcpy(output_filename, "output/");
		strcat(output_filename, argv[2]);

		if(strstr(argv[2], ".bin") != NULL)
			binary_output = 1;
	}
	else
		strcpy(output_filename, "stdout");
	
	int verbose = argc > 3 ? atoi(argv[3]) : 0;
	

	/* obsluga binarnego/tekstowego pliku wejsciowego */
	point_t lab_size, start, end;
	int start_left; /* zmienna przechowujaca info o kierunku wejscia do labiryntu*/
	if(strstr(input_filename, ".bin") != NULL){
		lab_info_binary(input, &lab_size, &start, &end);

		binary_to_txt(input, "lab.txt", lab_size);
		strcpy(input, "lab.txt");
	}
	else{
		lab_info_txt(input, &lab_size, &start, &end, &start_left);
	}
	int start_node = coords_to_node(start, lab_size);
	int end_node = coords_to_node(end, lab_size);
	point_t true_size = {(lab_size.x-1)/2, (lab_size.y-1)/2};

	if(verbose)
		printf("plik wczytany\n");


	/* utworzenie grafu na podst. labiryntu
	   i zapisanie go w pliku GRAPH_BIN */
	graph_to_bin_file(input, true_size);
	if(verbose)
		printf("graf zapisany w pliku binarnym\n");
	

	/* przejscie po labiryncie i zapisanie sciezki */
	traverse(start_node, end_node, true_size);
	if(verbose)
		printf("sciezka znaleziona\n");


	/* wypisanie rozwiazania */
	if(binary_output){
		compress_lab_to_binary(input, output_filename, lab_size, start, end);
		path_to_binary(output_filename, start_node, end_node, true_size);
	}
	else{
		path_to_txt(output_filename, start_node, end_node, true_size, start_left);
	}


	/* usuniecie plikow tymczasowych */
	if(strstr(input_filename, ".bin") != NULL)
		remove("lab.txt");
	delete_temp_files(TEMP_BIN_FILES, 4);
	if(verbose)
		printf("pliki tymczasowe usuniete\n");
	
	return 0;
}