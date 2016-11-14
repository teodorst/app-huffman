#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"

#define OPERATION 1
#define INPUT_FILE 2
#define OUTPUT_FILE 3
#define CODIFICATION 4
#define CHUNK 1048576
#define NR_BITS_CODE 1000	/* the maximum number of bits for a codification */


/* depth traversal of tree and build the code*/
void print_codes(struct node_t *root, int *vect, int top){
	int i;

    if (root -> left != NULL){
        vect[top] = 0;
        print_codes(root->left, vect, top + 1);
    }
 	
    if (root->right != NULL){
        vect[top] = 1;
        print_codes(root->right, vect, top + 1);
    }
 
 	/* if a leaf is found */
    if (!(root->left) && !(root->right)){

    	/* print the letter */
        printf("%c : ", root->data);

        /* print the code */
    	for (i = 0; i < top; ++i)
        	printf("%d", vect[i]);
    	printf("\n");
    }
}

struct node_t* build_huffman_tree(heap_t* h){
	/* TO DO : the first pop does not do anything*/
	// struct node_t* aux = (struct node_t*) malloc (sizeof(struct node_t));
	// aux = pop(h);
	// printf("in plus : %c \n", aux->data);

	while (h->len > 1){
		struct node_t* left = (struct node_t*) malloc (sizeof(struct node_t));
		struct node_t* right = (struct node_t*) malloc (sizeof(struct node_t));
		struct node_t* root = (struct node_t*) malloc (sizeof(struct node_t));

		/* extract two nodes from heap to form their root node */
		left = pop(h);
		right = pop(h);

		/* populate the root node */
		root -> priority = left->priority + right->priority;
		root -> data = '#';
		root -> left = left;
		root -> right = right;
		
		/* insert root node into the heap */
		push(h, root);
	}

	/* return the only node left in heap */
	return pop(h);
}

void insert_values_in_queue(unsigned long long int* frequecy, heap_t* h){
	int i;

	/* add all the letters into the heap */
	for (i = 0 ; i < 255 ; i++) {
		if (frequecy[i] > 0) {
			struct node_t* node = (struct node_t*) malloc (sizeof(struct node_t));
			node -> left = node -> right = NULL;
			node -> priority = frequecy[i];
			node -> data = i;
			push(h, node);
		}
	}

}

/*
void print_frequency(int chunk_size, unsigned long long int* frequecy){
	int i;
	
	for (i = 0 ; i < 255; i++){
		if (frequecy[i] != 0)
			printf("%llu ", frequecy[i]);
	}

	printf("\n");
}
*/

FILE* open_file(char *filename) {
	FILE* fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
        exit(EXIT_FAILURE);

    return fp;
}

void compute_frequency_for_chunk(unsigned char *chunk, int chunk_size, unsigned long long int* frequecy) {
	int i;

	for (i = 0; i < chunk_size; i ++) {
		frequecy[chunk[i]] ++;
	}

}

unsigned long long int* compute_frequency(FILE *fp) {
	size_t nread = 0;
	int chunk_size = 0;
	unsigned char buf[CHUNK];
	unsigned long long int* frequecy = (unsigned long long int*) calloc(255, sizeof(unsigned long long int));

	while((nread = fread(buf, 1, CHUNK, fp)) > 0) {
		chunk_size = CHUNK > nread ? nread : CHUNK;
		compute_frequency_for_chunk(buf, chunk_size, frequecy);
	//	print_frequency(chunk_size, frequecy);			
		memset(buf, CHUNK, '\0');
	}

	return frequecy;
}

int huffman_compress(char* input_filename, char* output_filename) {
	printf("compress\n");
	
	int floors[NR_BITS_CODE];
	int top = 0;

	FILE* fp = open_file(input_filename);
	unsigned long long int* frequecy = compute_frequency(fp);

	/* initialize and populate the priority queue */
	heap_t *h = init_priority_queue();
	insert_values_in_queue(frequecy, h);

	/* build the huffman tree */
	struct node_t *root = build_huffman_tree(h);

	/* print the encoded letters */
	print_codes(root, floors, top);

	return 0;
}

int huffman_decompress(char* input_filename, char* output_filename, char *codification) {
	printf("decompress\n");
	return 0;
}

int main (int argc, char* argv[]) {

	if (strcmp(argv[OPERATION], "compress") == 0 && argc == 4) {
		huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE]);
	}
	else if (strcmp(argv[OPERATION], "decompress") == 0 && argc == 5) {
		huffman_decompress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file [codification_file]\n");
		return 1;
	}

	return 0;
}


