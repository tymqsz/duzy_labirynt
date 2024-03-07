#ifndef IH
#define IH
#include "data.h"

void file_to_vec(char*, char**, box_t*);

void init_array_binary(char*, int, int);

void write_array_binary(char*, int*, int);

int* read_array_binary(char*, int, int);

void update_array_binary(char*, int, int);

int read_digit_binary(char*, int);
#endif
