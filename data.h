#ifndef DH
#define DH

typedef struct{
	int x;
	int y;
} point_t;

typedef struct{
	point_t A;
	point_t B;
} box_t;

char** zero_vec(point_t);

point_t* get_lab_info(char*);

box_t new_box(int, int, int, int);

box_t* get_division_points(point_t);

void free_vec(char**, point_t);

point_t biggest_box(box_t*);

int max(int, int);

int min(int, int);

#endif
