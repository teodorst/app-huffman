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
    printf("%d\n", myrank);


    char codification_v[CODIFICATION_V_SIZE];
    char** codification = (char**) calloc(128, sizeof(char*));


   	if (myrank == MASTER_PROCESS) {
		// open files
        FILE* input_file = open_file(input_filename, "r");
        FILE* output_file = open_file(output_filename, "w");
        FILE* codification_file = open_file(codification_filename, "w");

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

		int sum = 0;
		
		for (i = 0; i < 128; i ++) {
			sum += frequency[i];
            if (codification[i]) {
                printf("%c %s\n", i, codification[i]);
            }
		}
		printf("%d\n", sum);

        int codification_size = 0;
        for (i = 0; i < 128; i ++) {
            if (codification[i]) {
                codification_size ++;
            }
        }

        int codification_contor = 0;
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

        // for (i = 0; i < 128 * 20; i ++) {
        //     if (i % 20 == 0) {
        //         printf("\n");
        //     }
        //     printf("%c", codification_v[i]);
        // }
        // printf("second \n");
        // for (i = 0; i < 128; i ++) {
        //     printf("%s\n", codification_v + i * 20);
        // }
   	} else {

    }

    MPI_Bcast(codification_v, CODIFICATION_V_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (myrank == MASTER_PROCESS) {
        for (i = 1; i < nr_proc; i ++) {
            MPI_Send(&process_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(input_buffer + (i - 1) * process_size, process_size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            printf("%d a trimis la %d input buffer\n", myrank, i);
        }
    }

    if (myrank != MASTER_PROCESS) {
        
        for (i = 0; i < 128; i ++) {
            if (strlen(codification_v + i * 20) > 0) {
                codification[i] = strdup(codification_v + i * 20);
            }
        }

        // for (i = 0; i < 128; i++) {
        //     if (codification[i]) {
        //        printf("%c %s\n", i, codification[i]);
        //     }
        // }

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

    }



   	// else {
   	// 	input_buffer = (char *) malloc(process_size * sizeof(char));
    //     MPI_Recv(codification_v, CODIFICATION_V_SIZE, MPI_CHAR, MASTER_PROCESS, 0, MPI_COMM_WORLD, &status);
    //     printf("%d A primit codif\n", myrank );
   	// }

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
