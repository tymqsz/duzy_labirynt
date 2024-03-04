#include <stdlib.h>
#include <stdio.h>

#include "data.h"

void load_proper_block(point_t crt, box_t* boxes, FILE* f, int** lab, int* box){
	for(int i = 0; i < 9; i++){
		if((crt.x > boxes[i].A.x && crt.y > boxes[i].A.y && crt.y < boxes[i].B.y && crt.x < boxes[i].B.x)){
			file_to_vec(f, lab, boxes[i].A, boxes[i].B);
			*box = i;
			return;
		}
	}
	

	fprintf(stderr, "Couldnt load proper block\n");
	exit(1);
}

void traverse(int** lab, int** vis, point_t size, point_t end, box_t* boxes, FILE* f, int* box, point_t prev, point_t crt){
	if(crt.x == end.x && crt.y == end.y){
		printf("found a way out\n");
		exit(0);
	}
			
	// fix this shit
	printf("crt node: (%d, %d), (%d, %d)\n", crt.x, crt.y, crt.x-boxes[*box].A.x, crt.y-boxes[*box].A.y);
	
	// take direction vec ??
	point_t dir;
	dir.x = crt.x-prev.x;
	dir.y = crt.y-prev.y;
	
	if(crt.x >= size.x || crt.y >= size.y || crt.x < 1 || crt.y < 1)
		return;

	if(!(crt.x > boxes[*box].A.x && crt.x < boxes[*box].B.x && crt.y > boxes[*box].A.y && crt.y < boxes[*box].B.y)){
		load_proper_block(crt, boxes, f, lab, box);

		fclose(f);
		f = fopen("maze.txt", "r");
	}
	vis[crt.y][crt.x] = 1;
	

	if(crt.x >= size.x || crt.y >= size.y || crt.x < 1 || crt.y < 1)
		return;

	if(lab[crt.y-boxes[*box].A.y][crt.x-1-boxes[*box].A.x] && !vis[crt.y][crt.x-2]){
		point_t nxt1;
		nxt1.x = crt.x-2;
		nxt1.y = crt.y;
		
		traverse(lab, vis, size, end, boxes, f, box, crt, nxt1);
	}

	if(!(crt.x > boxes[*box].A.x && crt.x < boxes[*box].B.x && crt.y > boxes[*box].A.y && crt.y < boxes[*box].B.y)){
		load_proper_block(crt, boxes, f, lab, box);	
		fclose(f);
		f = fopen("maze.txt", "r");
	}
	if(lab[crt.y-boxes[*box].A.y][crt.x+1-boxes[*box].A.x] && !vis[crt.y][crt.x+2]){
		point_t nxt2;
		nxt2.x = crt.x+2;
		nxt2.y = crt.y;
		
		traverse(lab, vis, size, end, boxes, f, box, crt, nxt2);
	}
	if(!(crt.x > boxes[*box].A.x && crt.x < boxes[*box].B.x && crt.y > boxes[*box].A.y && crt.y < boxes[*box].B.y)){
		load_proper_block(crt, boxes, f, lab, box);	
		
		fclose(f);
		f = fopen("maze.txt", "r");
	}
	if(lab[crt.y-1-boxes[*box].A.y][crt.x-boxes[*box].A.x] && !vis[crt.y-2][crt.x]){
		point_t nxt3;
		nxt3.x = crt.x;
		nxt3.y = crt.y-2;
		
		traverse(lab, vis, size, end, boxes, f, box, crt, nxt3);
	}
	if(!(crt.x > boxes[*box].A.x && crt.x < boxes[*box].B.x && crt.y > boxes[*box].A.y && crt.y < boxes[*box].B.y)){
		load_proper_block(crt, boxes, f, lab, box);	
		fclose(f);
		f = fopen("maze.txt", "r");
	}
	if(lab[crt.y+1-boxes[*box].A.y][crt.x-boxes[*box].A.x] && !vis[crt.y+2][crt.x]){
		point_t nxt4;
		nxt4.x = crt.x;
		nxt4.y = crt.y+2;
		
		traverse(lab, vis, size, end, boxes, f, box, crt, nxt4);
	}
}
