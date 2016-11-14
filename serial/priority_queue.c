#include <stdio.h>
#include <stdlib.h>

#include "priority_queue.h"
#include "huffman.h"


heap_t* init_priority_queue() {
	heap_t* priority_queue = (heap_t*) calloc(1, sizeof(heap_t));
	return priority_queue;
}


void push(heap_t *heap, node_t* new_node) {
	if (heap->len == heap->size) {
		heap->size = heap->size ? heap->size * 2 : 4;
		heap->nodes = (node_t **) realloc(heap->nodes, heap->size * sizeof(node_t*));
	}
	
	int i, j;
	i = heap->len;
	j = i / 2;
	if (heap->len > 0) {
		
		while (i >= 0 && heap->nodes[j]->priority > new_node->priority && i != j) {
			heap->nodes[i] = heap->nodes[j];
			i = j;
			j /= 2;
		}
	}
	heap->nodes[i] = new_node;
	heap->len ++;
}


node_t* pop(heap_t* heap) {
	if (heap->len == 0) {
		return NULL;
	}

	int i, j, k;
	node_t* old_node = heap->nodes[0];
	heap->nodes[0] = heap->nodes[heap->len - 1];
	heap->len --;
	
	i = 0;
	while (1) {
		k = i;
		j = 2 * i + 1;
		if (j < heap->len && heap->nodes[j]->priority < heap->nodes[k]->priority) {
			k = j;
		}
		if (j + 1 < heap->len && heap->nodes[j + 1]->priority < heap->nodes[k]->priority) {
			k = j + 1;
		}
		if (i == k) {
			break;
		}
		heap->nodes[i] = heap->nodes[k];
		i = k;
		heap->nodes[i] = heap->nodes[heap->len];
	}

	return old_node;
}


void insert_values_in_queue(unsigned long long int* frequecy, heap_t* h){
	int i;

	/* add all the letters into the heap */
	for (i = 0 ; i < 255 ; i++) {
		if (frequecy[i] > 0) {
			node_t* node = (node_t*) malloc (sizeof(struct node_t));
			node->left = node->right = NULL;
			node->priority = frequecy[i];
			node->data = i;
			push(h, node);
		}
	}

}