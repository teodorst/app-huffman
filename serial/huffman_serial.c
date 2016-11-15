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
    
    char** codification = (char**) calloc(128, sizeof(char*));

    char path[MAX_BITS_CODE];

    FILE* input_fp = open_file(input_filename, "r");
    unsigned long long int* frequecy = compute_frequency(input_fp);

    /* build the huffman tree */
    node_t *root = build_huffman_tree(frequecy);

	// print the encoded letters 
    print_codes(root, path, 0);

    FILE* codification_fp = open_file(codification_filename, "w");

    /* write codification to file */
    find_codification(root, path, 0, codification);
    write_codification(codification_fp, codification);
	fclose(codification_fp);

	FILE* output_fp = open_file(output_filename, "w");

	write_codification_for_input_file(codification, input_fp, output_fp);

	fclose(input_fp);
	fclose(output_fp);

	// TODO free codification here

    return 0;
}

	

int huffman_decompress(char* input_filename, char* output_filename, char *codification_filename) {
    printf("decompress\n");

    FILE* codification_fp = open_file(codification_filename, "r");

    char** codification = read_configuration(codification_fp);
    int i;
    for (i = 0; i < 128; i ++) {
    	if (codification[i]) {
    		printf("%c : %s\n", i, codification[i]);
    	}
    }

    // TODO free codification
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
	return 0;
}


