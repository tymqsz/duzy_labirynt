#ifndef IH
#define IH
#include "data.h"

void get_lab_info(point_t* size);

void convert_to_txt(char* input, char* output, point_t lab_size);
void lab_to_array(char*, char**, block_t, point_t);

void load_proper_block(point_t, block_t*, char*, char**, int*, point_t);

void init_file_vector(char*, int, int);

int* read_file_vector(char*, int, int);

void update_file_vector(char*, int, int);

int read_file_position(char*, int);

void delete_temp_files(char**, int);
#endif
