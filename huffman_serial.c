#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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


void free_codification_matrix(char** codification) {
    int i;
    for (i = 0; i < 128; i ++) {
        if (codification[i]) {
            free(codification[i]);
        }
    }
    free(codification);
}


int huffman_compress(char* input_filename, char* output_filename, char* codification_filename) {
    printf("compress\n");
    
    char** codification = (char**) calloc(128, sizeof(char*));

    FILE* input_fp = open_file(input_filename, "r");
    FILE* codification_fp = open_file(codification_filename, "w");
    FILE* output_fp = open_file(output_filename, "w");

    // compute input file size
    fseek(input_fp, 0, SEEK_END);
    unsigned long long int size = ftell(input_fp);
    fseek(input_fp, 0, SEEK_SET);

    int i;
    char input_file_buffer[size];
    char output_file_buffer[size];

    // read input file //
    fread(input_file_buffer, 1, size, input_fp);

    unsigned long long int *frequency = (unsigned long long int*) calloc(128, sizeof(unsigned long long int));

    // compute frequency
    for (i = 0; i < size; i ++) {
        frequency[(unsigned char)input_file_buffer[i]] ++;
    } 
    
    // create huffman_tree
    node_t* root_holder = build_huffman_tree(frequency);
    char path[MAX_BITS_CODE];
    find_codification(root_holder, path, 0, codification);

    int output_buffer_contor = 0;
    char output_char = 0;
    int contor = 7;
    unsigned long long int bits = 0;

    time_t start = time(NULL);

    write_codification_for_chunk(input_file_buffer, size, codification, output_file_buffer, &output_buffer_contor, &output_char, &contor, &bits);
    
    printf("%.2f\n", (double)(time(NULL) - start));

    fwrite(output_file_buffer, 1, output_buffer_contor, output_fp);
    if (output_char != 0) {
        fwrite(&output_char, 1, 1, output_fp);
    }

    write_metadata_file_serial(codification_fp, codification, bits);

	fclose(input_fp);
	fclose(output_fp);
    fclose(codification_fp);

    free_codification_matrix(codification);
    return 0;
}


int huffman_decompress(char* input_filename, char* output_filename, char *codification_filename) {
    printf("decompress\n");
    unsigned long long int nbits = 0;

    FILE* codification_fp = open_file(codification_filename, "r");

    char** codification = read_configuration(codification_fp, &nbits);

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
    decode_bytes(input_fp, out_fp, root, nbits);


    free_codification_matrix(codification);
    return 0;
}



int main (int argc, char* argv[]) {

	if (argc == 5 && strcmp(argv[OPERATION], "compress") == 0) {
		huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else if (argc == 5 && strcmp(argv[OPERATION], "decompress") == 0) {
		huffman_decompress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file codification_file_name\n");
		return 1;
	}
	return 0;
}


