#ifndef FREQUENCY_H
#define FREQUENCY_H

#define CHUNK 1048576

void print_frequency(int chunk_size, unsigned long long int* frequecy);

void compute_frequency_for_chunk(unsigned char *chunk, int chunk_size, unsigned long long int* frequecy);

unsigned long long int* compute_frequency(FILE *fp);

#endif /* FREQUENCY_H */