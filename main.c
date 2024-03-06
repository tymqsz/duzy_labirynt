#include<stdio.h>
#include<stdlib.h>
#define MAX_NODES 1000
#include "data.h"
#include "algs.h"
#include "file_io.h"

char INPUT[] = "maze.txt";
char VISITED[] = "visited.txt";


int DEPTH = 0;

// Queue implementation
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}

int isQueueEmpty(struct Queue* queue) {
    return (queue->size == 0);
}

void enqueue(struct Queue* queue, int item) {
    if (queue->size == queue->capacity) {
        printf("Queue overflow\n");
        exit(EXIT_FAILURE);
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

int dequeue(struct Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Queue underflow\n");
        exit(EXIT_FAILURE);
    }
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

void MILION_MB_BFS(int** g, int start, int end, int* vis) {
    struct Queue* queue = createQueue(MAX_NODES);
    enqueue(queue, start);
    vis[start] = 1;  // Mark the starting node as visited

    while (!isQueueEmpty(queue)) {
        int crt = dequeue(queue);
        if (crt == end) {
            printf("reached the end\n");
            exit(EXIT_SUCCESS);
        }
        for (int j = 0; j < 4; j++) {
            if (g[crt][j] != 9999999 && !vis[crt + g[crt][j]]) {
                vis[crt + g[crt][j]] = 1;  // Mark the visited node
                enqueue(queue, crt + g[crt][j]);
            }
        }
        DEPTH++; // Increment the depth after processing each node
    }
}


int main(){
	// divide into two functions
	point_t* size_n_send = get_lab_info(INPUT);
	
	box_t* boxes = get_division_points(size_n_send[0]);
	point_t buffor_size = biggest_box(boxes);
	
	printf("buffor size: [%d, %d]\n", buffor_size.x, buffor_size.y);

	char** lab = zero_vec(buffor_size);
	
	point_t size = {size_n_send[0].x, size_n_send[0].y};
	point_t start = {1, 1};
	point_t end = {size.x-2, size.y-2};
	
	int box = 0;

	file_to_vec(INPUT, lab, &boxes[0]);
	
	point_t true_size = {(size.x-1)/2, (size.y-1)/2};
	extract_nodes(lab, true_size, start, end, boxes, INPUT, box);
	
	int* nodes = read_array_binary("neigh.bin", true_size.x*true_size.y*4-1, 1);
	printf("no. of nodes: %d\n", *nodes);
	
	int* array = read_array_binary("neigh.bin", 0, *nodes*4);
	
	exit(0);
	//int* vis = calloc(size.x*size.y, sizeof(int));
/*	
for(int i = 0; i < graph[size.x*size.y-1][0]; i++){
		for(int j = 0; j < 4; j++){
			printf("%d ", graph[i][j]);
		}
		printf("\n");
	}*/

	//MILION_MB_BFS(graph, 0, graph[size.x*size.y-1][0]-1, vis);

	/*
	init_visited_file(VISITED, size);


	// find block to load first based on start
	file_to_vec(INPUT, lab, &boxes[0]);

	int box = 0;
	//traverse2(lab, INPUT, boxes, box, start, end, size);
	traverse(lab, VISITED, INPUT, &box, boxes, &size, &end, &start);
	*/
	free_vec(lab, buffor_size);
}
