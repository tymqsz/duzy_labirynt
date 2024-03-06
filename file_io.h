#ifndef IH
#define IH
#include "data.h"

void change_file_position(char*, point_t*, point_t*, char);

char read_file_position(char*, point_t*, point_t*);

void init_visited_file(char*, point_t);

void file_to_vec(char*, char**, box_t*);

void init_queue(char*);

void append(char*, point_t);

point_t top(char*, int);

void init_array_binary(char*, int, int);

void write_array_binary(char*, int*, int);

int* read_array_binary(char*, int, int);

void update_array_binary(char*, int, int);
#endif
