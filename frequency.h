#ifndef FREQUENCY_H
#define FREQUENCY_H

#define CHUNK 104857600
// #define CHUNK 1073741824
// #define CHUNK 2
// #define CHUNK 1024


void print_frequency(int chunk_size, unsigned long long int* frequecy);


void compute_frequency_for_chunk(char *chunk, int start_index, int chunk_size, unsigned long long int* frequecy);

unsigned long long int* compute_frequency(FILE *fp);

#endif /* FREQUENCY_H */