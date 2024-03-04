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

int** zero_vec(point_t);

void file_to_vec(FILE*, int**, point_t, point_t);

point_t* get_lab_info(FILE*);

box_t new_box(int, int, int, int);

box_t* get_division_points(point_t size);

#endif
