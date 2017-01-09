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
#include <sys/stat.h>
#include <time.h>


#include "priority_queue.h"
#include "frequency.h"
#include "huffman.h"



#define INPUT_FILE 		1
#define OUTPUT_FILE 	2
#define CODIFICATION 	3

#define NUM_THREADS 	8
#define MASTER_THREAD   0


typedef struct huffman_thread_struct {
	int thread_id;
	char* input_buffer;
	char* output_buffer;
	unsigned long long int input_buffer_size;
    unsigned long long int *nbits_buffer;
	int* output_buffer_contors;
	char **codification;
} huffman_thread_struct;


FILE* open_file(char *filename, char *mode) {
	FILE* fp;
	fp = fopen(filename, mode);
	if (fp == NULL)
        exit(EXIT_FAILURE);

    return fp;
}

void *codification_thread(void *huffman_info_thread) {
	huffman_thread_struct* thread_arg = (huffman_thread_struct*) huffman_info_thread;
	
	unsigned long long int thread_size = thread_arg->input_buffer_size / NUM_THREADS;
	int index = (thread_arg->thread_id) * thread_size;
	int upper_limit = index + thread_size;
	
	int *output_buffer_contor = thread_arg->output_buffer_contors + thread_arg->thread_id;
	unsigned long long int *nbits = thread_arg->nbits_buffer + thread_arg->thread_id;

	
	char **codification = thread_arg->codification;

	char *input_buffer = thread_arg->input_buffer;
	char *output_buffer = thread_arg->output_buffer;

	printf("Hy! %d start %d, size %d !\n", thread_arg->thread_id, index, upper_limit);
	
	write_codification_for_chunk_pthreads(input_buffer, index, upper_limit, codification, output_buffer, output_buffer_contor, nbits);

	pthread_exit(NULL);
}

void huffman_codification_pthreads(char *input_file_name, char* output_file_name, char* codification_file_name) {
	
	// open files
	FILE* input_file = open_file(input_file_name, "r");
	FILE* output_file = open_file(output_file_name, "w");
	FILE* codification_file = open_file(codification_file_name, "w");

	// compute input file size
	fseek(input_file, 0, SEEK_END);
	unsigned long long int size = ftell(input_file);
	unsigned long long int thread_size = size / NUM_THREADS;
	fseek(input_file, 0, SEEK_SET);

	int i, ret;
	pthread_t threads[NUM_THREADS];
	huffman_thread_struct* arg_struct;
	
	char input_file_buffer[size];
	char output_file_buffer[size];

	
	int *output_buffer_contors = (int *) malloc(NUM_THREADS * sizeof(int));
	unsigned long long int *nbits_buffer = (unsigned long long int *) malloc(NUM_THREADS * sizeof(unsigned long long int));
	
	char** codification = (char**) calloc(128, sizeof(char*));


	// read input file //
	fread(input_file_buffer, 1, size, input_file);

	unsigned long long int *frequency = (unsigned long long int*) calloc(128, sizeof(unsigned long long int));

	// compute frequency
	for (i = 0; i < size; i ++) {
		frequency[(unsigned char)input_file_buffer[i]] ++;
	} 
	
	// create huffman_tree
	node_t* root_holder = build_huffman_tree(frequency);
	char path[MAX_BITS_CODE];
	find_codification(root_holder, path, 0, codification);

	time_t start = time(NULL);

	// create thrxeads
	for (i = 0; i < NUM_THREADS; i ++) {
		// create argument structure
		arg_struct = (huffman_thread_struct*) malloc(sizeof(huffman_thread_struct));
		arg_struct->thread_id = i;
		arg_struct->input_buffer = input_file_buffer;
		arg_struct->output_buffer = output_file_buffer;
		arg_struct->output_buffer_contors = output_buffer_contors;
		arg_struct->nbits_buffer = nbits_buffer;
		arg_struct->codification = codification;
		arg_struct->input_buffer_size = size;

		ret = pthread_create(&threads[i], NULL, codification_thread, (void *) arg_struct);
		if (ret) {
			printf("Error; return code from pthread_create() is %d\n", ret);
			exit(-1);
		}
	}

	for (i = 0; i < NUM_THREADS; i ++) {
		pthread_join(threads[i], NULL);
	}

	printf("%.2f\n", (double)(time(NULL) - start));


	for (i = 0; i < NUM_THREADS; i ++) {
		fwrite(output_file_buffer + i * thread_size, 1, output_buffer_contors[i], output_file);
		printf("%d\n", output_buffer_contors[i]);

	}

	write_metadata_file_pthreads(codification_file, codification, size, NUM_THREADS, nbits_buffer, output_buffer_contors);

	// close files
	fclose(input_file);
	fclose(output_file);
	fclose(codification_file);

	printf("Codification completed!\n");

}

int main (int argc, char *argv[]) {
	if (argc == 4) {
		fprintf(stderr, "%s %s %s\n", argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
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
