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
#define CODIFICATION_V_SIZE 128 * 20


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

int huffman_decompress(char* input_file_name, char* output_file_name, char *codification_file_name) {

	int myrank;
	int nr_proc;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_proc);

    FILE* codification_file = open_file(codification_file_name, "r");

    unsigned long long int nbits_buffer[nr_proc-1];
    int input_buffer_contors[nr_proc-1];

    size_t file_size = 0;
    int i, ret;
    char *buffer;
    int process_size = 0;
    char *input_buffer;
    char *output_buffer;

    char** codification = read_configuration_pthreads(codification_file, nbits_buffer, input_buffer_contors, &file_size);

    if (myrank == MASTER_PROCESS) {
    	FILE* input_file = open_file(input_file_name, "r");
	    FILE* output_file = open_file(output_file_name, "w");
    	
    	// compute input file size
		fseek(input_file, 0, SEEK_END);
		size_t compressed_file_size = ftell(input_file);
		int process_size = file_size / (nr_proc - 1);
		fseek(input_file, 0, SEEK_SET);


		int input_buffer_indexes[nr_proc-1];
		input_buffer_indexes[0] = 0;
		
		for (i = 1; i < nr_proc-1; i ++) {
			input_buffer_indexes[i] = input_buffer_indexes[i-1] + input_buffer_contors[i-1];
		}

	    input_buffer = (char*) malloc(file_size);
	    fread(input_buffer, 1, compressed_file_size, input_file);

	    for (i = 1; i < nr_proc; i ++) {
	    	MPI_Send(&process_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	    	MPI_Send(input_buffer_contors + i - 1, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	    	MPI_Send(nbits_buffer + i - 1, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
	    	MPI_Send(input_buffer + input_buffer_indexes[i-1], input_buffer_contors[i-1], MPI_CHAR, i, 0, MPI_COMM_WORLD);
	    	printf("%d a trimis toate 4 catre %d\n", myrank, i);
	    }

	    for (i = 1; i < nr_proc; i ++) {
	    	MPI_Recv(input_buffer + (i-1) * process_size, process_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
	    }

	   	fwrite(input_buffer, 1, file_size, output_file);

	    // close files
	    fclose(input_file);
	    fclose(output_file);
	    fclose(codification_file);

	}
    else {
    	
		/* for each codification find the leaf and save it into a buffer */
	    node_t* root = build_huffman_tree_from_codification(codification);
	    printf("Got the tree\n");

	    int input_buffer_contor = 0;
	    unsigned long long int nbits = 0;

	    MPI_Recv(&process_size, 1, MPI_INT, MASTER_PROCESS, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&input_buffer_contor, 1, MPI_INT, MASTER_PROCESS, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&nbits, 1, MPI_UNSIGNED_LONG, MASTER_PROCESS, 0, MPI_COMM_WORLD, &status);	 

		input_buffer = (char*) malloc(input_buffer_contor);
   		output_buffer = (char*) malloc(process_size);

	    MPI_Recv(input_buffer, input_buffer_contor, MPI_CHAR, MASTER_PROCESS, 0, MPI_COMM_WORLD, &status);
	   	printf("%d a primit toate 4 de la %d: %d %d %llu\n", myrank, MASTER_PROCESS, process_size, input_buffer_contor, nbits);


	    decode_bytes_for_chunk_pthreads(root, input_buffer, nbits, output_buffer);

	    MPI_Send(output_buffer, process_size, MPI_CHAR, MASTER_PROCESS, 0, MPI_COMM_WORLD);
    }


	return 0;
}


int main (int argc, char* argv[]) {
	
	MPI_Init (&argc, &argv);

	if (argc == 4) {
		huffman_decompress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file codification_file_name\n");
		MPI_Finalize();
		return 1;
	}
	MPI_Finalize();
	return 0;
}