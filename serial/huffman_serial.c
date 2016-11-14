#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"
#include "frequency.h"
#include "huffman.h"

#define OPERATION 1
#define INPUT_FILE 2
#define OUTPUT_FILE 3
#define CODIFICATION 4


FILE* open_file(char *filename, char *mode) {
	FILE* fp;
	fp = fopen(filename, mode);
	if (fp == NULL)
        exit(EXIT_FAILURE);

    return fp;
}



int huffman_compress(char* input_filename, char* output_filename, char* codification_filename) {
    printf("compress\n");
    
    int floors[MAX_BITS_CODE];

    FILE* input_fp = open_file(input_filename, "r");
    unsigned long long int* frequecy = compute_frequency(input_fp);

    /* build the huffman tree */
    node_t *root = build_huffman_tree(frequecy);

    /* print the encoded letters */
    print_codes(root, floors, 0);

    FILE* codification_fp = open_file(codification_filename, "w");
    
    /* write codification to file */
    write_codification(codification_fp, root, floors, 0);

    fclose(input_fp);
    fclose(codification_fp);

    return 0;
}


int huffman_decompress(char* input_filename, char* output_filename, char *codification) {
    printf("decompress\n");
    return 0;
}


int main (int argc, char* argv[]) {

	if (strcmp(argv[OPERATION], "compress") == 0 && argc == 5) {
		huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else if (strcmp(argv[OPERATION], "decompress") == 0 && argc == 5) {
		huffman_decompress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file codification_file_name\n");
		return 1;
	}
	FILE *dec = open_file("text.", "w");
	int i;
	for ( i = 0 ; i < 128; i ++) {
		fputc(i, dec);
	}
	fclose(dec);
	return 0;
}


