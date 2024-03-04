#ifndef AH
#define AH

#include "data.h"

void load_proper_block(point_t, box_t*, FILE*, int**, int*);

void traverse(int** lab, int** vis, point_t size, point_t end, box_t* boxes, FILE* f, int*, point_t, point_t crt);

#endif
