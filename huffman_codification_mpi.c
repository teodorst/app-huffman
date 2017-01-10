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

int huffman_compress(char* input_filename, char* output_filename, char* codification_filename) {

	int myrank;
	int nr_proc;
	MPI_Status status;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nr_proc);

	char *input_buffer;

	int size;
	int process_size;
    int i;
	// read file only by master process


    char codification_v[CODIFICATION_V_SIZE];
    char** codification = (char**) calloc(128, sizeof(char*));

    FILE* input_file;
    FILE* output_file;
    FILE* codification_file;

   	if (myrank == MASTER_PROCESS) {
		// open files
        input_file = open_file(input_filename, "r");
        output_file = open_file(output_filename, "w");
        codification_file = open_file(codification_filename, "w");

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
			frequency[(unsigned char)input_buffer[i]] ++;
		}
        
        // create huffman_tree
        node_t* root_holder = build_huffman_tree(frequency);
        char path[MAX_BITS_CODE];
        find_codification(root_holder, path, 0, codification);

        // transform the codification matrix into a vector 
        int row, i;
        for (row = 0; row < 128; row++)
        {
            if (codification[row]) {
                for (i = 0; i < strlen(codification[row]) ; i++)
                    codification_v[row * 20 + i] = codification[row][i];
                
                for (i = strlen(codification[row]); i < 20; i++)
                    codification_v[row * 20 + i] = '\0';
                }
            else {
                for (i = 0; i < 20; i++)
                    codification_v[row * 20 + i] = '\0';
            }
        }
   	} 

    MPI_Bcast(codification_v, CODIFICATION_V_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (myrank == MASTER_PROCESS) {
        for (i = 1; i < nr_proc; i ++) {
            MPI_Send(&process_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(input_buffer + (i - 1) * process_size, process_size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            printf("%d a trimis la %d input buffer\n", myrank, i);
        }

        int output_buffer_contors[nr_proc - 1];
        unsigned long long int nbits_buffer[nr_proc - 1];
        for (i = 1; i < nr_proc; i ++) {
            MPI_Recv(output_buffer_contors + i - 1, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(nbits_buffer + i - 1, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(input_buffer + (i - 1) * process_size, process_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
        }

        for (i = 0; i < nr_proc-1; i++) {
            fwrite(input_buffer + i * process_size, 1, output_buffer_contors[i], output_file);
        }

        write_metadata_file_pthreads(codification_file, codification, size, nr_proc-1, nbits_buffer, output_buffer_contors);

        // close files
        fclose(input_file);
        fclose(output_file);
        fclose(codification_file);
    }

    if (myrank != MASTER_PROCESS) {
        
        for (i = 0; i < 128; i ++) {
            if (strlen(codification_v + i * 20) > 0) {
                codification[i] = strdup(codification_v + i * 20);
            }
        }

    
        MPI_Recv(&process_size, 1, MPI_INT, MASTER_PROCESS, 0, MPI_COMM_WORLD, &status);
        printf("%d \n", process_size);
        input_buffer = (char *) malloc(process_size * sizeof(char));
        char* output_buffer = (char *) malloc(process_size * sizeof(char));

        MPI_Recv(input_buffer, process_size, MPI_CHAR, MASTER_PROCESS, 0, MPI_COMM_WORLD, &status);
        
        printf("%d a primit\n", myrank);

        int output_buffer_contor = 0;
        unsigned long long int nbits = 0;
        write_codification_for_chunk_pthreads(input_buffer, 0, process_size, codification, output_buffer, &output_buffer_contor, &nbits);

        printf("%d a codificat in %d chars %llu bits\n", myrank, output_buffer_contor, nbits);

        MPI_Send(&output_buffer_contor, 1, MPI_INT, MASTER_PROCESS, 0, MPI_COMM_WORLD);
        MPI_Send(&nbits, 1, MPI_UNSIGNED_LONG, MASTER_PROCESS, 0, MPI_COMM_WORLD);
        MPI_Send(output_buffer, output_buffer_contor, MPI_CHAR, MASTER_PROCESS, 0, MPI_COMM_WORLD);

    }

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
        MPI_Finalize();
	return 1;
    }

    MPI_Finalize();
    return 0;
}
