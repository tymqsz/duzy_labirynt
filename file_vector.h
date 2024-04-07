#ifndef FVH
#define FVH

void init_file_vector(char*, int, int);

int* read_file_vector(char*, int, int);

void update_file_vector(char*, int, int);

int read_file_position(char*, int);

void delete_temp_files(char**, int);

#endif