#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_BITS_CODE 127	/* the maximum number of bits for a codification */
#define BYTE_SIZE 8
#define OUTPUT_BUFFER_SIZE 1024

#define MPI_TAG 10

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


void write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp, FILE* codification_fp, FILE *codification_fp2, int myrank, int nr_proc);

void write_codification_for_chunk(char *chunk, int chunk_size, char **codification, FILE* output_fp, FILE* codification_fp, int myrank);

void write_codification(FILE* codification_fp, char **codification);

char** read_configuration(FILE *codification_fp);

void find_codification(node_t *root, char *path, int level, char** codification);

void print_codes(node_t *root, char *path, int top);

node_t* build_huffman_tree(unsigned long long int* frequency);

node_t* build_huffman_tree_from_codification(char **codification);

void decode_bytes(FILE *in_fp, FILE *out_fp, FILE *codification_fp, node_t *root, unsigned long long int *bits_per_chunk);

#endif /* HUFFMAN_H */
