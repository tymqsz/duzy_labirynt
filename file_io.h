#ifndef IH
#define IH
#include "data.h"

void path_to_txt(int, int, point_t);

void path_to_binary(int, int, point_t);

void compress_lab_to_binary(char*, point_t, point_t, point_t);

void init_file_vector(char*, int, int);

int* read_file_vector(char*, int, int);

void update_file_vector(char*, int, int);

int read_file_position(char*, int);

void delete_temp_files(char**, int);
#endif
