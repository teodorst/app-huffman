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

void write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp);

void write_codification_for_chunk(char *chunk, int chunk_size, char **codification, FILE* output_fp, char* output_char, int* contor, 
	unsigned long long int* bits);

void write_codification(FILE* codification_fp, char **codification);

char** read_configuration(FILE *codification_fp);

void find_codification(node_t *root, char *path, int level, char** codification);

void print_codes(node_t *root, char *path, int top);

node_t* build_huffman_tree(unsigned long long int* frequecy);

#endif /* HUFFMAN_H */
