#include <stdio.h>
#include <stdlib.h>

#include "priority_queue.h"

heap_t* init_priority_queue() {
	heap_t* priority_queue = (heap_t*) calloc(1, sizeof(heap_t));
	return priority_queue;
}


void push(heap_t *heap, struct node_t* new_node) {
	if (heap->len == heap->size) {
		heap->size = heap->size ? heap->size * 2 : 4;
		heap->nodes = (struct node_t *) realloc(heap->nodes, heap->size * sizeof(struct node_t));
	}
	
	int i, j;
	i = heap->len;
	j = i / 2;
	while (i >= 0 && heap->nodes[j].priority > new_node->priority && i != j) {
		heap->nodes[i] = heap->nodes[j];
		i = j;
		j /= 2;
	}
	heap->nodes[i].priority = new_node->priority;
	heap->nodes[i].data = new_node->data;
	heap->nodes[i].left = new_node->left;
	heap->nodes[i].right = new_node->right;
	heap->len ++;
}

struct node_t* pop(heap_t* heap) {
	if (heap->len == 0) {
		struct node_t* null_node = (struct node_t*) malloc (sizeof(struct node_t));
		null_node->data = '$';
		null_node->priority = -1;
		null_node->left = NULL;
		null_node->right = NULL;
		return null_node;
//		return NULL;
	}
	int i, j, k;

	struct node_t* old_node = (struct node_t *) malloc (sizeof(struct node_t));
	old_node -> data = heap->nodes[0].data;
	old_node -> priority = heap->nodes[0].priority;
	old_node -> left = heap->nodes[0].left;
	old_node -> right = heap->nodes[0].right;

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

	return old_node;
}