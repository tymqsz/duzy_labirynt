#include<stdio.h>
#include<stdlib.h>

#include "data.h"
#include "algs.h"
#include "file_io.h"
#include "queue.h"
#include "metadata.h"

char INPUT[] = "maze.txt";

char* TEMP[] = {QUEUE_BIN, GRAPH_BIN, PARENT_BIN, PATH_BIN}; /* sciezki do plikow tymczasowych */

int main(int argc, char** argv){
	int verbose = 0;
	if(argc > 1)
		verbose = atoi(argv[1]);
	
	/* znalezienie rozmiaru pliku wejsciowego i labiryntu */
	point_t lab_size = get_lab_size(INPUT);
	point_t true_size = {(lab_size.x-1)/2, (lab_size.y-1)/2};
	
	/* podzielenie labiryntu na 9 czesci */
	block_t* blocks = divide_into_9_blocks(lab_size);
	point_t buffor_size = biggest_block(blocks);
	if(verbose)
		printf("labirynt podzielony, najwiekszy blok: %dkb\n", buffor_size.x*buffor_size.y/1000);
	
	/* zapisanie informacji o labiryncie w pliku binarnym */
	lab_to_bin_file(INPUT, true_size, blocks, buffor_size);
	if(verbose)
		printf("labirynt przepisany do pliku binarnego\n");
	
	/* przejscie po labiryncie i zapisanie sciezki */
	int start_node = 0, end_node = true_size.x*true_size.y - 1;
	traverse(start_node, end_node, true_size);
	
	/* usuniecie plikow tymczasowych */
	delete_temp_files(TEMP, 4);
	if(verbose)
		printf("pliki tymczasowe usuniete\n");
	
	return 0;
}
