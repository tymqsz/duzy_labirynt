#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "data.h"
#include "bfs.h"
#include "file_vector.h"
#include "file_io.h"
#include "queue.h"
#include "metadata.h"

char* TEMP_BIN_FILES[] = {QUEUE_BIN, GRAPH_BIN, PARENT_BIN, PATH_BIN}; /* sciezki do plikow tymczasowych */

int main(int argc, char** argv){
	char* input_filename = NULL;
    char* output_filename = "stdout";
    int verbose = 0;

    /* uzycie getopt do wczytania argumentow */ 
    int opt;
    while ((opt = getopt(argc, argv, "i:o:v")) != -1) {
        switch (opt) {
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
            default: /* '?' */
                fprintf(stderr, "Uzycie: %s -i [pliku wejsciowy] -o [plik wyjsciowy]"
				                " -v [0/1(wypisywanie komunikatow)]\nwymagany argument: -i\n", argv[0]);
				return -1;
		}
    }
	/* upewnienie sie ze foldery input/output istnieja */
    DIR *input_dir = opendir("input");
	DIR *output_dir = opendir("output");
    if(!input_dir){
		printf("folder \"input\" nie istnieje\n");
		return 13;
	}
	if(!output_dir)
		mkdir("output", 0777);
	
	/* sprawdzenie czy podano plik wejsciowy */
	char input[30];
	if(input_filename == NULL){
		printf("nie podano pliku wejsciowego\n");
		return 17;
	}
	strcpy(input, "input/");
	strcat(input, input_filename);
	
	char output[30];
	int binary_output = 0;
	if(strstr(output_filename, "stdout") == NULL){
		strcpy(output, "output/");
		strcat(output, output_filename);

		if(strstr(output_filename, ".bin") != NULL)
			binary_output = 1;
	}
	else
		strcpy(output, output_filename);

	/* obsluga binarnego/tekstowego pliku wejsciowego */
	point_t lab_size, start, end;
	int start_left; /* zmienna przechowujaca info o kierunku wejscia do labiryntu*/
	int input_status;
	if(strstr(input_filename, ".bin") != NULL){
		input_status = lab_info_binary(input, &lab_size, &start, &end, &start_left);
		
		binary_to_txt(input, "lab.txt", lab_size);
		strcpy(input, "lab.txt");

		printf("start: [%d, %d], end: [%d, %d]\n", start.x, start.y, end.x, end.y);
	}
	else{
		input_status = lab_info_txt(input, &lab_size, &start, &end, &start_left);
		
	}
	if(input_status == 1){
		printf("nie moge otworzyc pliku wejsciowego\n");
		return 2137;
	}
	else if(input_status == 2){
		printf("niepoprawny format pliku wejsciowego\n");
		return 19;
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
	int no_path;
	no_path = traverse(start_node, end_node, true_size);
	if(no_path){
		printf("brak sciezki w labiryncie\n");
		if(strstr(input_filename, ".bin") != NULL)
			//remove("lab.txt");
		delete_temp_files(TEMP_BIN_FILES, 4);
		if(verbose)
			printf("pliki tymczasowe usuniete\n");
		return 11;
	}
	if(verbose)
		printf("sciezka znaleziona\n");


	/* wypisanie rozwiazania */
	int output_status;
	if(binary_output){
		output_status = compress_lab_to_binary(input, output, lab_size, start, end);
		path_to_binary(output, start_node, end_node, true_size);
	}
	else{
		output_status = path_to_txt(output, start_node, end_node, true_size, start_left);
	}
	if(output_status == 1){
		printf("nie moge utworzyc pliku wyjsciowego\n");
		return 69;
	}


	/* usuniecie plikow tymczasowych */
	if(strstr(input_filename, ".bin") != NULL)
		//remove("lab.txt");
	delete_temp_files(TEMP_BIN_FILES, 4);
	if(verbose)
		printf("pliki tymczasowe usuniete\n");
	
	return 0;
}
