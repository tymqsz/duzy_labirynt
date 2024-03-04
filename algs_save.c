#include <stdlib.h>
#include <stdio.h>

#include "data.h"

void traverse(int** lab, int** vis, point_t size, point_t end, point_t crt){
	if(crt.x == end.x && crt.y == end.y)
		printf("found a way out\n");
	
	vis[crt.y][crt.x] = 1;
	
	// dont make many copies of crt
	if(lab[crt.y][crt.x-1] && !vis[crt.y][crt.x-2]){
		point_t nxt1;
		nxt1.x = crt.x-2;
		nxt1.y = crt.y;
		
		traverse(lab, vis, size, end, nxt1);
	}
	if(lab[crt.y][crt.x+1] && !vis[crt.y][crt.x+2]){
		point_t nxt2;
		nxt2.x = crt.x+2;
		nxt2.y = crt.y;
		
		traverse(lab, vis, size, end, nxt2);
	}
	if(lab[crt.y-1][crt.x] && !vis[crt.y-2][crt.x]){
		point_t nxt3;
		nxt3.x = crt.x;
		nxt3.y = crt.y-2;
		
		traverse(lab, vis, size, end, nxt3);
	}
	if(lab[crt.y+1][crt.x] && !vis[crt.y+2][crt.x]){
		point_t nxt4;
		nxt4.x = crt.x;
		nxt4.y = crt.y+2;
		
		traverse(lab, vis, size, end, nxt4);
	}
}

