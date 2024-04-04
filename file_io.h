#ifndef IH
#define IH
#include "data.h"

void lab_info_binary(char*, point_t* size);

void binary_to_txt(char* input, char* output, point_t lab_size);

void init_file_vector(char*, int, int);

int* read_file_vector(char*, int, int);

void update_file_vector(char*, int, int);

int read_file_position(char*, int);

void delete_temp_files(char**, int);
#endif
