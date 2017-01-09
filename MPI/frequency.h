#ifndef FREQUENCY_H
#define FREQUENCY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define CHUNK 1048576
//#define CHUNK 5
//#define CHUNK 1024


void print_frequency(int chunk_size, unsigned long long int* frequecy);

void compute_frequency_for_chunk(char *chunk, int chunk_size, unsigned long long int* frequecy);

unsigned long long int* compute_frequency(FILE *fp);

#endif /* FREQUENCY_H */