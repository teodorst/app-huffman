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

int huffman_compress(char* input_filename, char* output_filename, char* codification_filename) {
    printf("compress\n");
    
    char** codification = (char**) calloc(128, sizeof(char*));

    char path[MAX_BITS_CODE];

    FILE* input_fp = open_file(input_filename, "r");
    unsigned long long int* frequency = compute_frequency(input_fp);

    /* build the huffman tree */
    node_t *root = build_huffman_tree(frequency);

    /* print the encoded letters */ 
    // print_codes(root, path, 0);
    
    find_codification(root, path, 0, codification);


    FILE* codification_fp = open_file(codification_filename, "w");
    FILE* output_fp = open_file(output_filename, "w");

    write_codification_for_input_file(codification, input_fp, output_fp);
    int i = 0;
    int sum = 0;
    for ( i = 0 ; i < 128; i ++) {
        sum += frequency[i];
        printf("%llu ", frequency[i]);
    }
    printf("\n");
    printf("%d\n", sum);
    fclose(input_fp);
    fclose(output_fp);

    /* write codification to file */
    write_codification(codification_fp, codification);
    fclose(codification_fp);

    free_codification_matrix(codification);
    return 0;
}


int main (int argc, char* argv[]) {

    if (argc == 4) {
        huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
    }
    else {
        printf("Bad arguments\n");
        printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file codification_file_name\n");
        return 1;
    }
    return 0;
}