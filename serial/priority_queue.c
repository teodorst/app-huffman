#include <stdio.h>
#include <stdlib.h>

#include "priority_queue.h"

heap_t* init_priority_queue() {
	heap_t* priority_queue = (heap_t*) calloc(1, sizeof(heap_t));
	return priority_queue;
}


void push(heap_t *heap, int priority, char* data) {
	if (heap->len == heap->size) {
		heap->size = heap->size ? heap->size * 2 : 4;
		heap->nodes = (node_t *) realloc(heap->nodes, heap->size * sizeof(node_t));
	}
	
	int i, j;
	i = heap->len;
	j = i / 2;
	while (i >= 0 && heap->nodes[j].priority > priority && i != j) {
		heap->nodes[i] = heap->nodes[j];
		i = j;
		j /= 2;
	}
	heap->nodes[i].priority = priority;
	heap->nodes[i].data = data;
	heap->len ++;
}

char* pop(heap_t* heap) {
	if (heap->len == 0) {
		return NULL;
	}
	int i, j, k;

	char *data = heap->nodes[0].data;
	heap->nodes[0] = heap->nodes[heap->len - 1];
	heap->len --;
	i = 0;
	while (1) {
		k = i;
		j = 2 * i + 1;
		// printf("Comparam %d %d %d\n", heap->nodes[j].priority, heap->nodes[j+1].priority, heap->nodes[k].priority);
		if (j < heap->len && heap->nodes[j].priority < heap->nodes[k].priority) {
			// printf("1\n");
			k = j;
		}
		if (j + 1 < heap->len && heap->nodes[j + 1].priority < heap->nodes[k].priority) {
			// printf("2\n");
			k = j + 1;
		}
		if (i == k) {
			break;
		}
		heap->nodes[i] = heap->nodes[k];
		i = k;
			heap->nodes[i] = heap->nodes[heap->len];

		// printf("I %d K: %d Nodes[i] %d \n", i, k, heap->nodes[i].priority);

	}

	return data;
}