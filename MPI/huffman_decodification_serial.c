#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"
#include "frequency.h"
#include "huffman.h"

#define INPUT_FILE 1
#define OUTPUT_FILE 2
#define CODIFICATION 3

FILE* open_file(char *filename, char *mode) {
	FILE* fp;
	fp = fopen(filename, mode);
	if (fp == NULL)
        exit(EXIT_FAILURE);

    return fp;
}


void free_codification_matrix(char** codification) {
    int i;
    for (i = 0; i < 128; i ++) {
        if (codification[i]) {
            free(codification[i]);
        }
    }
    free(codification);
}

int huffman_decompress(char* input_filename, char* output_filename, char *codification_filename) {
    printf("decompress\n");
    long int nr_chunks;

    FILE* codification_fp = open_file(codification_filename, "r");
     FILE* codification_fp2 = open_file("metadata", "r");
    // read the bits for each chunk
    fscanf(codification_fp2, "%ld\n", &nr_chunks);

    // read the bits for each chunk
    unsigned long long int *bits_per_chunk = (unsigned long long int *) malloc (nr_chunks * sizeof(unsigned long long int)); 
    int i;
    for (i = 0 ; i < nr_chunks ; i++)
        fscanf(codification_fp2, "%llu\n", &bits_per_chunk[i]);

    // read the codification
    char** codification = read_configuration(codification_fp);

    /* for each codification find the leaf and save it into a buffer */
    node_t* root = build_huffman_tree_from_codification(codification);
    printf("Got the tree\n");
    // int top = 0;
    // char path[MAX_BITS_CODE];
    // print_codes(root, path, top);
    
    /* read the bytes from the inputfile,  */
    FILE* input_fp = open_file(input_filename, "r");
    FILE* out_fp = open_file(output_filename, "w");
    printf("Opened files\n");

    /* decode the bytes using huffman tree */
    decode_bytes(input_fp, out_fp, codification_fp, root, bits_per_chunk);

    fclose(input_fp);
    fclose(out_fp);
    fclose(codification_fp);
    fclose(codification_fp2);

    free_codification_matrix(codification);
    return 0;
}

int main (int argc, char* argv[]) {
	
	if (argc == 4) {
		huffman_decompress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file codification_file_name\n");
		return 1;
	}
	return 0;
}
