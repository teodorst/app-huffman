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


node_t* init_node(char data);


unsigned long long int write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp);


void write_codification_for_chunk(char *chunk, int chunk_size, char **codification, char* output_buffer, 
    int *output_buffer_contor, char* output_char, int* contor, unsigned long long int* bits);


void write_codification_for_chunk_pthreads(char *chunk, int index, int chunk_size, char **codification, 
    char *output_buffer, int *output_buffer_contor, unsigned int *bits);


void write_metadata_file_serial(FILE* codification_fp, char **codification, unsigned long long int nbits);

void write_metadata_file_pthreads(FILE* codification_fp, char **codification, unsigned int chunks_no);

void write_codification(FILE* codification_fp, char **codification);


char** read_configuration(FILE *codification_fp, unsigned long long int *nbits);


void find_codification(node_t *root, char *path, int level, char** codification);


void print_codes(node_t *root, char *path, int top);


node_t* build_huffman_tree(unsigned long long int* frequency);


node_t* build_huffman_tree_from_codification(char **codification);


void decode_bytes(FILE *in_fp, FILE *out_fp, node_t *root, unsigned long long int nbits);


#endif /* HUFFMAN_H */
