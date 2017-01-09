#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "priority_queue.h"
#include "frequency.h"
#include "huffman.h"

#define INPUT_FILE 1
#define OUTPUT_FILE 2
#define CODIFICATION 3

#define TAG 10
#define MASTER_PROCESS 0

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

	int myrank;
	int nr_proc;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_proc);

	char *input_buffer;

	unsigned long long int size;
	unsigned long long int process_size;

	// read file only by master process
   	if (myrank = MASTER_PROCESS) {
		FILE* input_file;
	    FILE* output_file;
	    FILE* codification_file;

	    // compute input file size
		fseek(input_file, 0, SEEK_END);
		size = ftell(input_file);
		process_size = size / (nr_proc-1);
		fseek(input_file, 0, SEEK_SET);

   		input_buffer = (char *) malloc(size * sizeof(char));

   		fread(input_buffer, 1, size, input_file);

   		unsigned long long int *frequency = (unsigned long long int*) calloc(128, sizeof(unsigned long long int));
   		// compute frequency
		for (i = 0; i < size; i ++) {
			frequency[(unsigned char)input_file_buffer[i]] ++;
		}

		int sum = 0;
		
		for (i = 0; i < 128; i ++) {
			sum += frequency[i];
		}

		printf("%d\n", sum);

   	}
   	else {
   		input_buffer = (char *) malloc(process_size * sizeof(char));
   	}

    char** codification = (char**) calloc(128, sizeof(char*));
    char *codification_v = (char *) malloc (128 * 128 * sizeof(char*));
    char path[MAX_BITS_CODE];

    // // the master task
    // if (myrank == 0){
    //     input_fp = open_file(input_filename, "r");
    //     unsigned long long int* frequecy = compute_frequency(input_fp);

    //     /* build the huffman tree */
    //     node_t *root = build_huffman_tree(frequecy);

    //     /* print the encoded letters */ 
    //     // print_codes(root, path, 0);

    //     find_codification(root, path, 0, codification);
        
    //     // transform the codification matrix into a vector 
    //     int row, i;
    //     for (row = 0; row < 128; row++)
    //     {
    //         if (codification[row]){
    //         for (i = 0; i < strlen(codification[row]) ; i++)
    //             codification_v[row * 128 + i] = codification[row][i];
            
    //         for (i = strlen(codification[row]); i < 128; i++)
    //             codification_v[row * 128 + i] = '2';
    //         }
    //         else
    //         {
    //             for (i = 0; i < 128; i++)
    //             codification_v[row * 128 + i] = '2';
    //         }
    //     }

    // }

    // // broadcast the codification matrix to all procs
    // MPI_Bcast(codification_v, 128 * 128, MPI_CHAR, 0, MPI_COMM_WORLD);

    // // transform the codification vector back into a matrix
    // if (myrank != 0) {
    //     int i, j;

    //     for (i = 0 ; i < 128 ; i++)
    //     {
    //         if (codification_v[i * 128] == '2')
    //             continue;
            
    //         codification[i] =(char *) malloc (128 * sizeof(char));
    //         for (j = 0; j < 128 ; j++)
    //             if (codification_v[i*128+j] != '2')
    //                 codification[i][j] = codification_v[i*128+j];
    //             else
    //                 codification[i][j] = '\0';
    //     }
    // }

    // // open files for write
    // if (myrank == 0){
    //     codification_fp = open_file(codification_filename, "w");
    //     codification_fp2 = open_file("metadata", "w");
    //     output_fp = open_file(output_filename, "w");
    // }

    // //encode the content 
    // write_codification_for_input_file(codification, input_fp, output_fp, codification_fp, codification_fp2, myrank, nr_proc);

    // if (myrank == 0) {
    //      /* write codification to file */
    //     write_codification(codification_fp, codification);

    //     fclose(input_fp);
    //     fclose(output_fp);
    //     fclose(codification_fp);
    // }

    // free_codification_matrix(codification);
    MPI_Finalize();

    return 0;
}


int main (int argc, char* argv[]) {

	MPI_Init (&argc, &argv);
    if (argc == 4) {
        huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
    }
    else {
        printf("Bad arguments\n");
        printf("Usage huffman_serial_codification input_file ouput_file codification_file_name\n");
	return 1;
    }

    return 0;
}
