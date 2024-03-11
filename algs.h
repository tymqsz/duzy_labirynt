#ifndef AH
#define AH

#include "data.h"

void load_proper_block(point_t*, block_t*, char*, char**, int*);

void extract_nodes(char**, point_t, point_t, point_t, block_t*, char*, int);

void bfs(int, int, int, point_t);

#endif
