#ifndef DH
#define DH

/* struktura reprezentujaca
   punkt w 2d */
typedef struct{
	int x;
	int y;
} point_t;

/* struktura reprezentujaca 
   blok labiryntu */
typedef struct{
	point_t mini; /* minimalne wspolrzedne pola labiryntu */
	point_t maxi; /* maskymalne ------------||----------- */
} block_t;

char** zero_vec(point_t);

point_t get_lab_size(char*);

block_t new_block(int, int, int, int);

block_t* divide_into_9_blocks(point_t);

void free_vec(char**, point_t);

point_t biggest_block(block_t*);

int max(int, int);

int min(int, int);

#endif
