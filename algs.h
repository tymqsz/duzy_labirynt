#ifndef AH
#define AH

#include "data.h"

void load_proper_block(point_t*, box_t*, char*, char**, int*);

void extract_nodes(char** lab, point_t size, point_t start, point_t end, box_t* boxes, char* filename, int box);

#endif
