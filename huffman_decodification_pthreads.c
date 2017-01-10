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

#define NUM_THREADS 	4
#define MASTER_THREAD   0

typedef struct huffman_thread_struct {
    int thread_id;
    char* input_buffer;
    int* input_indexes;
    char* output_buffer;
    int output_buffer_size;
    unsigned long long int *nbits_buffer;
    int* output_buffer_contors;
    node_t* root;
} huffman_thread_struct;



FILE* open_file(char *filename, char *mode) {
    FILE* fp;
    fp = fopen(filename, mode);
    if (fp == NULL)
        exit(EXIT_FAILURE);

    return fp;
}


void *decodification_thread(void *huffman_info_thread) {
    huffman_thread_struct* thread_arg = (huffman_thread_struct*) huffman_info_thread;
    int id = thread_arg->thread_id;

    int index_input = thread_arg->input_indexes[id];
    int thread_size_output = thread_arg->output_buffer_size / NUM_THREADS;
    int index_output = id * thread_size_output;
    unsigned long long int nbits = thread_arg->nbits_buffer[id];
    
    char *input_buffer = thread_arg->input_buffer;
    char *output_buffer = thread_arg->output_buffer;

    printf("Hy! %d start %d, size %d !\n", id, index_input, index_output);

    decode_bytes_for_chunk_pthreads(thread_arg->root, input_buffer + index_input, nbits, output_buffer + index_output);

    pthread_exit(NULL);
}



void huffman_decodification_pthreads(char *input_file_name, char* output_file_name, char* codification_file_name) {
    // open file_size
    FILE* input_file = open_file(input_file_name, "r");
    FILE* output_file = open_file(output_file_name, "w");
    FILE* codification_file = open_file(codification_file_name, "r");

    unsigned long long int nbits_buffer[NUM_THREADS];
    int input_buffer_contors[NUM_THREADS];

    size_t file_size = 0;
    int i, ret;
    size_t compressed_file_size = 0;

    char** codification = read_configuration_pthreads(codification_file, nbits_buffer, input_buffer_contors, &file_size);
    
    for (i = 0; i < NUM_THREADS; i ++) {
        compressed_file_size += input_buffer_contors[i];
    }

    for (i = NUM_THREADS - 1; i > 0; i --) {
        input_buffer_contors[i] = input_buffer_contors[i-1];
    }
    input_buffer_contors[0] = 0;

    for (i = 1; i < NUM_THREADS; i ++) {
        input_buffer_contors[i] += input_buffer_contors[i-1];
    }

    /* for each codification find the leaf and save it into a buffer */
    node_t* root = build_huffman_tree_from_codification(codification);
    printf("Got the tree\n");

    char input_buffer[compressed_file_size];
    char output_buffer[file_size];
    fread(input_buffer, 1, compressed_file_size, input_file);
   
    huffman_thread_struct* arg_struct;
    pthread_t threads[NUM_THREADS];

    char path[100];
    print_codes(root, path, 0);

    time_t start = time(NULL);

    printf("Incepe durerea\n");
    // create threads
    for (i = 0; i < NUM_THREADS; i ++) {
        // create argument structure
        arg_struct = (huffman_thread_struct*) malloc(sizeof(huffman_thread_struct));
        arg_struct->thread_id = i;
        arg_struct->input_buffer = input_buffer;
        arg_struct->input_indexes = input_buffer_contors;
        arg_struct->output_buffer = output_buffer;
        arg_struct->output_buffer_size = file_size;
        arg_struct->nbits_buffer = nbits_buffer;
        arg_struct->root = root;

        ret = pthread_create(&threads[i], NULL, decodification_thread, (void *) arg_struct);
        if (ret) {
            printf("Error; return code from pthread_create() is %d\n", ret);
            exit(-1);
        }
    }


    for (i = 0; i < NUM_THREADS; i ++) {
        pthread_join(threads[i], NULL);
    }

    printf("%.2f\n", (double)(time(NULL) - start));

    fwrite(output_buffer, 1, file_size, output_file);

    // close files
    fclose(input_file);
    fclose(output_file);
    fclose(codification_file);

}


int main (int argc, char *argv[]) {
    if (argc == 4) {
        fprintf(stderr, "%s %s %s\n", argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
        huffman_decodification_pthreads(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
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
