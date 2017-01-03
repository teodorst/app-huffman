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

//procesul cu rank-ul 0 stabileste fracventele, contruieste arborele huffman si creaza tabela de mapare
if (myrank == 0){
    FILE* input_fp = open_file(input_filename, "r");
    unsigned long long int* frequecy = compute_frequency(input_fp);

    /* build the huffman tree */
    node_t *root = build_huffman_tree(frequecy);

    /* print the encoded letters */ 
    // print_codes(root, path, 0);

    
    find_codification(root, path, 0, codification);
}
// trimiterea broadcast de la procesul cu rank 0 la celelalte procese numarul de coloane - MPI_Bcast
MPI_Bcast(&size_codification, 1, MPI_INT, 0, MPI_COMM_WORLD);

if (myrank != 0){
//se aloca mem pentru matricea codification

}

// trimitere broadcast de la procesul cu rank 0 la celelalte procese - MPI_Bcast 
MPI_Bcast(&(codification[0][0]), 128 * size_codification, MPI_CHAR, 0, MPI_COMM_WORLD);

// procesul cu rank-ul 0 este singurul care va scrie in fisier
if (myrank == 0){
    FILE* codification_fp = open_file(codification_filename, "w");
    FILE* output_fp = open_file(output_filename, "w");

    write_codification_for_input_file(codification, input_fp, output_fp);

    fclose(input_fp);
    fclose(output_fp);
}

// procesul cu rank-ul zero va trimite fiecarui alt proces un chunk din fisier pentru care se va face encodarea
// celelalte procese fac prelucrarile necesare si un MPI_Send cu informatiile obtinute catre 0; 
// procesul master va scrie in fisier rezultatele fiecarui proces
    /* write codification to file */
    write_codification(codification_fp, codification);

if (myrank == 0){
    fclose(codification_fp);
}

    free_codification_matrix(codification);
    return 0;
}


int main (int argc, char* argv[]) {
	int myrank, size;
	int tag = 25;
	MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc == 4) {
        huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
    }
    else {
        printf("Bad arguments\n");
        printf("Usage huffman_serial_codification input_file ouput_file codification_file_name\n");
        MPI_Finalize();
	return 1;
    }

	MPI_Finalize();
    return 0;
}
