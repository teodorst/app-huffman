#include <omp.h>

#ifndef FREQUENCY_H
#define FREQUENCY_H

#define CHUNK 10000000 // 10MB
//  #define CHUNK 1048576
// #define CHUNK 2
// #define CHUNK 1024


void print_frequency(int* frequecy);

void compute_frequency_for_chunk(char *chunk, int chunk_size, int* frequecy);

int *compute_frequency_buffer(char *buffer, int size);

int* compute_frequency(FILE *fp);

char *read_file(FILE *fp, long file_size);

#endif /* FREQUENCY_H */