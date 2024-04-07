#ifndef IH
#define IH
#include "data.h"

int lab_info_txt(char*, point_t*, point_t*, point_t*, int*);

int lab_info_binary(char*, point_t*, point_t*, point_t*);

int path_to_txt(char*, int, int, point_t, int);

void path_to_binary(char*, int, int, point_t);

int compress_lab_to_binary(char*, char*, point_t, point_t, point_t);

#endif
