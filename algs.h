#ifndef AH
#define AH

#include "data.h"

#define MAX_SIZE 100

void load_proper_block(point_t*, box_t*, char*, char**, int*);

void traverse(char**, char*, char*, int*, box_t*, point_t*, point_t*, point_t*);

void traverse2(char** lab, char* INPUT, box_t* boxes, int box, point_t start, point_t end, point_t size);

void extract_nodes(char** lab, point_t size, point_t start, point_t end, box_t* boxes, char* filename, int box);
#endif
