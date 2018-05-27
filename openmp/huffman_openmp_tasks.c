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
#define WORKER_NO 5


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


int huffman_compress(char* input_filename, char* output_filename, char* codification_filename, int workers_no) {
    int i;
    char** codification = (char**) calloc(128, sizeof(char*));
    int codification_size = 0;
    char path[MAX_BITS_CODE];

    FILE* input_fp = open_file(input_filename, "r");
    long file_size = get_file_length(input_fp);
    int tasks_num = file_size % CHUNK == 0 ? file_size / CHUNK : file_size / CHUNK + 1;

    char *input_data = read_file(input_fp, file_size);
    if (input_data == NULL) {
        return -1;
    }
    char *output_data = (char *)calloc(file_size, sizeof(char));
    int* frequency = compute_frequency_buffer(input_data, file_size);
    
    /* build the huffman tree */
    node_t *root = build_huffman_tree(frequency);

	/* print the encoded letters */ 
    // print_codes(root, path, 0);    
    find_codification(root, path, 0, codification);
    for (i = 0; i < 128; i ++) {
        if (codification[i] != NULL) {
            codification_size ++;
        }
    }
    FILE* codification_fp = open_file(codification_filename, "w");
    FILE* output_fp = open_file(output_filename, "w");
    int *output_buffer_contors = (int*) calloc(tasks_num, sizeof(int));
    long *nbits_buffer = (long*) calloc(tasks_num, sizeof(long));
    
    omp_set_num_threads(workers_no);
    #pragma omp parallel
    {
        #pragma omp single 
        {
            int i;
            for (i = 0; i < tasks_num; i ++) {
                #pragma omp task firstprivate(i) shared(file_size, input_data, output_data, nbits_buffer)
                {
                    int start_index = i * CHUNK;
                    int finish_index = start_index + CHUNK > file_size ? file_size : start_index + CHUNK;
                    int output_buffer_contor = 0;
                    long nbits = 0;
                    write_codification_for_chunk_tasks(input_data, start_index, finish_index, codification, 
                        output_data, &output_buffer_contor, &nbits);
                    output_buffer_contors[i] = output_buffer_contor;
                    nbits_buffer[i] = nbits;
                }
            }
        }
    }

    for (i = 0; i < tasks_num; i ++) {
        fwrite(output_data+ i * CHUNK, output_buffer_contors[i], sizeof(char), output_fp);
    }

	fclose(input_fp);
	fclose(output_fp);

    /* write codification to file */
    write_codification_metadata(codification_fp, file_size, codification, codification_size, output_buffer_contors, nbits_buffer, tasks_num);
    fclose(codification_fp);

    free_codification_matrix(codification);
    return 0;
}

int huffman_decompress(char* input_filename, char* output_filename, char *codification_filename, int workers_no) {
    int i;
    int file_size = 0;
    int tasks_num = 0;
    long nbits = 0L;
    int *input_buffer_contors;
    int compressed_file_size = 0;
    long *nbits_buffer;

    FILE* input_fp = open_file(input_filename, "r");
    FILE* output_fp = open_file(output_filename, "w");
    FILE* codification_fp = open_file(codification_filename, "r");
    
    char** codification = read_configuration_metadata(codification_fp, &file_size, &input_buffer_contors, 
        &nbits_buffer, &tasks_num, &nbits, &compressed_file_size);
    char *input_data = read_file(input_fp, compressed_file_size);
    char *output_data = (char*) calloc(file_size, sizeof(char));
    
    // printf("File size %d\n", file_size);
    // printf("Nbits file %ld\n", nbits);
    // printf("Tasks num %d\n", tasks_num);
    // for (i = 0; i < tasks_num; i ++) {
    //     printf("Task %d bytes %d and bits %ld\n", i, input_buffer_contors[i], nbits_buffer[i]);
    // }
    
    /* for each codification find the leaf and save it into a buffer */
    node_t* root = build_huffman_tree_from_codification(codification);
    if (root == NULL) {
        printf("Tree is not formed\n");
        exit(1);
    }
    // printf("Got the tree\n");
    // int top = 0;
    // char path[MAX_BITS_CODE];
    // print_codes(root, path, top);
    
    /* decode the bytes using huffman tree */
    omp_set_num_threads(workers_no);
    #pragma omp parallel
    {
        #pragma omp single 
        {
            int i;
            for (i = 0; i < tasks_num; i ++) {
                #pragma omp task firstprivate(i) shared(input_data, output_data, input_buffer_contors, nbits_buffer)
                {
                    int output_index = i * CHUNK;
                    decode_bytes_tasks(root, input_data + input_buffer_contors[i], nbits_buffer[i], output_data + output_index);
                }
            }
        }
    }

    fwrite(output_data, file_size, sizeof(char), output_fp);

    free_codification_matrix(codification);
    return 0;
}

int main (int argc, char* argv[]) {    
    double start_time = omp_get_wtime();
	if (argc == 6 && strcmp(argv[OPERATION], "compress") == 0) {
		huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION], atoi(argv[WORKER_NO]));
	}
	else if (argc == 6 && strcmp(argv[OPERATION], "decompress") == 0) {
		huffman_decompress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION], atoi(argv[WORKER_NO]));
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file codification_file_name\n");
		return 1;
	}
    printf("%.2f\n", omp_get_wtime()-start_time);
	return 0;
}


