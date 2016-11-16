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
    
    unsigned long long int nbits;
    char** codification = (char**) calloc(128, sizeof(char*));

    char path[MAX_BITS_CODE];

    FILE* input_fp = open_file(input_filename, "r");
    unsigned long long int* frequecy = compute_frequency(input_fp);

    /* build the huffman tree */
    node_t *root = build_huffman_tree(frequecy);

	/* print the encoded letters */ 
    print_codes(root, path, 0);
    
    find_codification(root, path, 0, codification);


    FILE* codification_fp = open_file(codification_filename, "w");
	FILE* output_fp = open_file(output_filename, "w");

	nbits = write_codification_for_input_file(codification, input_fp, output_fp);

	fclose(input_fp);
	fclose(output_fp);

    /* write codification to file */
    write_codification(codification_fp, codification, nbits);
    fclose(codification_fp);

    free_codification_matrix(codification);
    return 0;
}

	

int huffman_decompress(char* input_filename, char* output_filename, char *codification_filename) {
    printf("decompress\n");
    unsigned long long int nbits = 0;

    FILE* codification_fp = open_file(codification_filename, "r");

    char** codification = read_configuration(codification_fp, &nbits);
    node_t* root = build_huffman_tree_from_codification(codification);
    
    int top = 0;
    char path[MAX_BITS_CODE];
    print_codes(root, path, top);
    
    // citim mai multi bytes, parcurgem byte cu byte. si facem dfs-uri in functie de fiecare bit al fiecarui byte.
    // ar trebui sa mearga destul de repede. trebuie sa verificam sa avem citirea buna.

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


