// barriers

#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"
#include "frequency.h"
#include "huffman.h"



#define INPUT_FILE 		1
#define OUTPUT_FILE 	2
#define CODIFICATION 	3

#define NUM_THREADS 	4
#define MASTER_THREAD   0

typedef struct huffman_thread_struct {
	int thread_id;
	char* input_buffer;
    pthread_barrier_t *frequency_barrier;
    int *nread;
	node_t** root_holder;
	FILE* input_file;
	FILE* output_file;
	FILE* codification_file;
	unsigned long long int **frequencies;
} huffman_thread_struct;


FILE* open_file(char *filename, char *mode) {
	FILE* fp;
	fp = fopen(filename, mode);
	if (fp == NULL)
        exit(EXIT_FAILURE);

    return fp;
}


void read_from_file_thread(FILE* input_file, char *input_buffer, int* nread, unsigned long long int* frequency) {
	memset(input_buffer, '\0', CHUNK);
	*nread = fread(input_buffer, sizeof(char), CHUNK, input_file);
	if (*nread < CHUNK) {
		compute_frequency_for_chunk(input_buffer, 0, *nread, frequency);
	}
}


void read_from_file_thread_and_apply_codification(FILE* input_file, char *input_buffer, int* nread) {
	memset(input_buffer, '\0', CHUNK);
	*nread = fread(input_buffer, sizeof(char), CHUNK, input_file);
	if (*nread < CHUNK) {

	}
}


void apply_huffman(char *input_buffer, int start_index, int size, char *output_buffer, char **codification) {
	int current_index = 0;
	int i = 0;
	memset(output_buffer, '\0', 8 * CHUNK);
	for (i = start_index; i < start_index + size; i ++) {
		char *codif = codification[(unsigned char)input_buffer[i]];
		strcpy(output_buffer + current_index, codif);
		current_index += strlen(codif);
	}
}

void *codification_thread(void *huffman_info_thread) {
	huffman_thread_struct* thread_arg = (huffman_thread_struct* ) huffman_info_thread;
	unsigned long long int* frequency = thread_arg->frequencies[thread_arg->thread_id];
	int size = CHUNK / NUM_THREADS;
	int index = (thread_arg->thread_id) * size;

	// printf("Hy! %d start %d, size %d !\n", thread_arg->thread_id, index, size, thread_arg->frequencies[thread_arg->thread_id]);
	
	// first read
	if (thread_arg->thread_id == MASTER_THREAD) {
		read_from_file_thread(thread_arg->input_file, thread_arg->input_buffer, thread_arg->nread, frequency);
	}
	
	pthread_barrier_wait(thread_arg->frequency_barrier);
	while(*(thread_arg->nread) == CHUNK) {
		compute_frequency_for_chunk(thread_arg->input_buffer, index, size, frequency);
		printf("%d!!!\n", *(thread_arg->nread));
		// only master thread will run here
		pthread_barrier_wait(thread_arg->frequency_barrier);
		if (thread_arg->thread_id == MASTER_THREAD) {
			read_from_file_thread(thread_arg->input_file, thread_arg->input_buffer, thread_arg->nread, frequency);
			// printf("%d\n", *(thread_arg->nread));
		}
		pthread_barrier_wait(thread_arg->frequency_barrier);
	}
	
	// sum up all frequencies and rewind input file pointer
	int i, j;
	if (thread_arg->thread_id == MASTER_THREAD) {
		for (j = 0; j < 128; j ++) {
			for (i = 1; i < NUM_THREADS; i ++) {
				frequency[j] += thread_arg->frequencies[i][j];
			}
		}
		fseek(thread_arg->input_file, 0, SEEK_SET);
		
		// create huffman_tree
		*(thread_arg->root_holder) = build_huffman_tree(frequency);
		char** codification = (char**) calloc(128, sizeof(char*));
		char path[MAX_BITS_CODE];
		find_codification(*(thread_arg->root_holder), path, 0, codification);
		// write_codification(thread_arg->codification_file, codification);

	}

	pthread_barrier_wait(thread_arg->frequency_barrier);

	// codifcation of input



	pthread_exit(NULL);
}

void huffman_codification_pthreads(char *input_file_name, char* output_file_name, char* codification_file_name) {
	// open files
	FILE* input_file = open_file(input_file_name, "r");
	FILE* output_file = open_file(output_file_name, "w");
	FILE* codification_file = open_file(codification_file_name, "w");

	// barriers
	pthread_barrier_t frequency_barrier;
	if(pthread_barrier_init(&frequency_barrier, NULL, NUM_THREADS))
    {
        printf("Could not create a barrier\n");
        exit(-1);
    }

	int i, ret;
	int nread = 0;
	pthread_t threads[NUM_THREADS];
	huffman_thread_struct* arg_struct;
	char input_file_buffer[CHUNK];
	unsigned long long int **frequencies;


	// init tree placeholder
	node_t **root_holder = (node_t**) malloc(sizeof(node_t*));


	// init frequencies matrix
	frequencies = (unsigned long long int **) malloc(NUM_THREADS * sizeof(unsigned long long int*));
	for (i = 0; i < NUM_THREADS; i ++) {
		frequencies[i] = (unsigned long long int*) calloc(128, sizeof(unsigned long long int));
	}

	// create threads
	for (i = 0; i < NUM_THREADS; i ++) {
		// create argument structure
		arg_struct = (huffman_thread_struct*) malloc(sizeof(huffman_thread_struct));
		arg_struct->thread_id = i;
		arg_struct->input_buffer = input_file_buffer;
		arg_struct->root_holder = root_holder;
		arg_struct->input_file = input_file;
		arg_struct->output_file = output_file;
		arg_struct->codification_file = codification_file;
		arg_struct->frequency_barrier = &frequency_barrier;
		arg_struct->nread = &nread;
		arg_struct->frequencies = frequencies;

		ret = pthread_create(&threads[i], NULL, codification_thread, (void *) arg_struct);
		if (ret) {
			printf("Error; return code from pthread_create() is %d\n", ret);
			exit(-1);
		}

	}

	for (i = 0; i < NUM_THREADS; i ++) {
		pthread_join(threads[i], NULL);
	}

	// close files
	fclose(input_file);
	fclose(output_file);
	fclose(codification_file);

	printf("Codification completed!\n");

}

int main (int argc, char *argv[]) {
	if (argc == 4) {
		huffman_codification_pthreads(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file codification_file_name\n");
		return 1;
	}
	return 0;
	/* Last thing that main() should do */
	pthread_exit(NULL);
}