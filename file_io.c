#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "metadata.h"
#include "file_vector.h"

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

int reverse_path(int start_node, int end_node, point_t true_size){
	int crt = end_node;
	int read;
	 
	init_file_vector(PATH_BIN, true_size.x*true_size.y, -1); /* inicjalizacja pliku binarnego
																do przechowywania sciazki */
	/* przepisanie sciezki do pliku binarnego */
	int node_cnt = 0;
	while(crt != start_node){
		read = read_file_position(PARENT_BIN, crt);
		 
		update_file_vector(PATH_BIN, node_cnt, crt);
		
		crt = read;
		node_cnt++;
	}
	update_file_vector(PATH_BIN, node_cnt, crt);

	return node_cnt;
}

/* funkcja zapisujaca znaleziona sciezke do pliku */
void path_to_txt(char* output_filename, int start_node, int end_node, point_t true_size, int start_left){
	int node_cnt = reverse_path(start_node, end_node, true_size);	
	
	FILE* output;
	if(strstr(output_filename, "stdout") == NULL)
		output = fopen(output_filename, "ab");
	else
		output = stdout;

	int x, y, prev_x, prev_y, steps = 0;
	point_t dir;
	int dir_index, prev_dir_index;
	char turn[20];
	
	/* zapisanie sciezki w postaci krokow do pliku path.txt */
	int crt = read_file_position(PATH_BIN, node_cnt);
	prev_x = crt % true_size.x;
	prev_y = crt / true_size.x;
	node_cnt--;

	if(start_left)
		prev_dir_index = 2;
	else
		prev_dir_index = 3;
	while(node_cnt >= 0){
		crt = read_file_position(PATH_BIN, node_cnt);
		/* obliczenie koordynatow danego wierzcholka */
		x = crt % true_size.x; 
		y = crt / true_size.x;
		
		/* obliczenie wektora przesuniecia na podst.
		   pozycji aktualnej i poprzedniej */
		dir.x = x - prev_x;
		dir.y = y - prev_y;
		
		dir_index = get_dir_index(dir); /* sprawdzenie indeksu wektora przesuniecia */
		
		/* jesli kierunek ten sam co poprzednio, zwiekszenie liczby krokow */
		if(prev_dir_index == dir_index)
			steps++;
		else{
			/* w przeciwnym wypadku sprawdzenie kierunku skretu i wypisanie liczby krokow */
			if(dir_index - prev_dir_index == 1 || dir_index - prev_dir_index == -3)
				strcpy(turn, "TURNRIGHT");
			else
				strcpy(turn, "TURNLEFT");
			
			if(steps > 0)
				fprintf(output, "FORWARD %d\n%s\n", steps, turn);
			else
				fprintf(output,"%s\n", turn);

			steps = 1;
		}

		prev_dir_index = dir_index;
		prev_x = x;
		prev_y = y;

		node_cnt--;
	}
	/* wypisanie ostatniej prostej */
	fprintf(output, "FORWARD %d\n", steps);
	fclose(output);
}

void path_to_binary(char* output_filename, int start_node, int end_node, point_t true_size){
	int node_cnt = reverse_path(start_node, end_node, true_size);	
	
	int x, y, prev_x, prev_y;
	point_t dir;
	int dir_index, prev_dir_index = -1;
	FILE* output;
	if(strstr(output_filename, "stdout") == NULL)
		output = fopen(output_filename, "ab");
	else
		output = stdout;
	
	/* zliczenie liczby krokow */
	unsigned short int step_cnt = 0;
	int node = node_cnt;
	int crt = read_file_position(PATH_BIN, node_cnt);
	prev_x = crt % true_size.x;
	prev_y = crt / true_size.x;
	node--;
	while(node >= 0){
		crt = read_file_position(PATH_BIN, node);
		
		x = crt % true_size.x; 
		y = crt / true_size.x;
		
		/* obliczenie wektora przesuniecia na podst.
		   pozycji aktualnej i poprzedniej */
		dir.x = x - prev_x;
		dir.y = y - prev_y;
		
		dir_index = get_dir_index(dir); /* sprawdzenie indeksu wektora przesuniecia */
		
		if(prev_dir_index != dir_index && prev_dir_index != -1)
			step_cnt++;

		prev_dir_index = dir_index;
		prev_x = x;
		prev_y = y;
		node--;
	}
	
	/* naglowek rozwiazania */
	unsigned int id_rozw = 0x52524243;
	fwrite(&id_rozw, 4, 4, output);
	fwrite(&step_cnt, 2, 2, output); /* 2 bajty zamiast 1 */
	
	
	char char_dir;
	unsigned char step = 0;
	prev_dir_index = -1;

	/* wypisanie kolejnych krokow rozwiazania */
	node = node_cnt;
	crt = read_file_position(PATH_BIN, node_cnt);
	prev_x = crt % true_size.x;
	prev_y = crt / true_size.x;
	node--;
	while(node >= 0){
		crt = read_file_position(PATH_BIN, node);
		
		x = crt % true_size.x; 
		y = crt / true_size.x;
		
		dir.x = x - prev_x;
		dir.y = y - prev_y;
		
		dir_index = get_dir_index(dir);
		
		if(prev_dir_index != dir_index && prev_dir_index != -1){
			switch(prev_dir_index){
			    case 0:
					char_dir = 'W';
					break;
				case 1:
					char_dir = 'N';
					break;
				case 2:
					char_dir = 'E';
					break;
				case 3:
					char_dir = 'S';
					break;
			}
			
			/* zapisanie kroku */
			fwrite(&char_dir, 1, 1, output);
			fwrite(&step, 1, 1, output);

			step = 1;
		}
		else
			step++;

		prev_dir_index = dir_index;
		prev_x = x;
		prev_y = y;
		node--;
	}
	switch(prev_dir_index){
		case 0:
			char_dir = 'W';
			break;
		case 1:
			char_dir = 'N';
			break;
		case 2:
			char_dir = 'E';
			break;
		case 3:
			char_dir = 'S';
			break;
	}

	/*ostatnia prosta*/
	fwrite(&char_dir, 1, 1, output);
	fwrite(&step, 1, 1, output);
	fclose(output);
}

void compress_lab_to_binary(char* input_filename, char* output_filename, point_t lab_size, point_t start, point_t end){
	FILE* in = fopen(input_filename, "r");
	FILE* output;
	if(strstr(output_filename, "stdout") == NULL)
		output = fopen(output_filename, "ab");
	else
		output = stdout;
	
	/* znalezienie liczby slow kodowych */
	int slowa_kodowe_cnt = 0;
	int i = 0;
	unsigned char reps = 0;
	char prev_c = '.';
	char c;
	while(i < lab_size.x*lab_size.y){
		c = fgetc(in);
		
		if(c == '\n')
			continue;
		
		if((prev_c != '.' && prev_c != c) || reps == 255){
			slowa_kodowe_cnt++;
			reps = 0;
		}
		else if(prev_c == '.' || prev_c == c)
			reps++;
		i++;
	}
	
	/* wypisanie naglowka */
	unsigned int file_id = 0x52524243;
	char esc = 0x1B;
	int reserved = 0;
	int solution_offset = 0;
	char sep = '*', wall = 'X', path = ' ';

	fwrite(&file_id, 4, 4, output);
	fwrite(&esc, 1, 1, output);
	fwrite(&reserved, 4, 4, output);
	fwrite(&reserved, 4, 4, output);
	fwrite(&reserved, 4, 4, output);
	fwrite(&slowa_kodowe_cnt, 4, 4, output);
	fwrite(&solution_offset, 4, 4, output);
	fwrite(&sep, 1, 1, output);
	fwrite(&wall, 1, 1, output);
	fwrite(&path, 1, 1, output);


	
	/* wypisane kolejnych slow kodowych */
	i = 0;
	reps = 0;
	prev_c = '.';
	while(i < lab_size.x*lab_size.y){
		c = fgetc(in);
		
		if(c == '\n')
			continue;
		
		if((prev_c != '.' && prev_c != c) || reps == 255){
			/*wypisanie slowa kodowego */
			fwrite(&sep, 1, 1, output);
			fwrite(&prev_c, 1, 1, output);
			fwrite(&reps, 1, 1, output);

			reps = 0;
		}
		else if(prev_c == '.' || prev_c == c)
			reps++;
		i++;
	}

	fclose(in);
	fclose(output);
}