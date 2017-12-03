#include <omp.h>

#ifndef HUFFMAN_H
#define HUFFMAN_H

#define MAX_BITS_CODE 127	/* the maximum number of bits for a codification */
#define BYTE_SIZE 8

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


unsigned long long int write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp);

void write_codification_for_chunk(char *chunk, int chunk_size, char **codification, FILE* output_fp, char* output_char, int* contor, 
	unsigned long long int* bits);

void write_codification_metadata(FILE* codification_fp, int file_size, char **codification, int codification_size, int *output_buffer_contors, 
	long *nbits_buffer, int task_num);

char **read_configuration(FILE *codification_fp, unsigned long long int *nbits);

char **read_configuration_metadata(FILE* codification_fp, int *file_size, int **input_buffer_contors, long **nbits_buffer, 
	int *tasks_num, long *nbits, int *compressed_file_size);

void find_codification(node_t *root, char *path, int level, char** codification);

void print_codes(node_t *root, char *path, int top);

node_t* build_huffman_tree(int* frequency);

node_t* build_huffman_tree_from_codification(char **codification);

void decode_bytes(FILE *in_fp, FILE *out_fp, node_t *root, unsigned long long int nbits);

long get_file_length(FILE *fp);

void write_codification_for_chunk_tasks(char *chunk, int index, int upper_limit, char **codification,
	char *output_buffer, int *output_buffer_contor, long *bits);

int decode_bytes_tasks(node_t *root, char *buffer, unsigned long long int nbits, char* out_buffer);

#endif /* HUFFMAN_H */
