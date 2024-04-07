#ifndef DH
#define DH

typedef struct{
	int x;
	int y;
} point_t;


void lab_info_txt(char*, point_t*, point_t*, point_t*, int*);

void lab_info_binary(char*, point_t*, point_t*, point_t*);

int coords_to_node(point_t, point_t);

void binary_to_txt(char*, char*, point_t);

void graph_to_bin_file(char*, point_t);

int max(int, int);

int min(int, int);

#endif
