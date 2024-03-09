#ifndef IH
#define IH
#include "data.h"

void lab_to_vec(char*, char**, block_t);

void init_file_vector(char*, int, int);

int* read_file_vector(char*, int, int);

void update_file_vector(char*, int, int);

int read_file_position(char*, int);

void delete_temp_files(char**, int);
#endif
