#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "huffman.h"

heap_t* init_priority_queue();

void push(heap_t *heap, struct node_t *node);

struct node_t* pop(heap_t* heap);

void insert_values_in_queue(unsigned long long int* frequecy, heap_t* h);

#endif /* PRIORITY_QUEUE_H */