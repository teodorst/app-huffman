#ifndef HUFFMAN_H
#define HUFFMAN_H

#define MAX_BITS_CODE 127	/* the maximum number of bits for a codification */

typedef struct node_t{
	int priority;
	char data;
	struct node_t* left;
	struct node_t* right;
} node_t;


typedef struct {
    struct node_t **nodes;
    int len;
    int size;
} heap_t;


void write_codification(FILE* codification_fp, node_t *root, int *vect, int top);

void print_codes(node_t *root, int *vect, int top);

node_t* build_huffman_tree(unsigned long long int* frequecy);

#endif /* HUFFMAN_H */
