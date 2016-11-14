#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frequency.h"

void print_frequency(int chunk_size, unsigned long long int* frequency){
	int i;
	
	for (i = 0 ; i < 128; i++){
		if (frequency[i] != 0)
			printf("%llu ", frequency[i]);
	}

	printf("\n");
}


void compute_frequency_for_chunk(unsigned char *chunk, int chunk_size, unsigned long long int* frequency) {
	int i;

	for (i = 0; i < chunk_size; i ++) {
		frequency[chunk[i]] ++;
	}

}


unsigned long long int* compute_frequency(FILE *fp) {
	size_t nread = 0;
	int chunk_size = 0;
	unsigned char buf[CHUNK];
	unsigned long long int* frequency = (unsigned long long int*) calloc(128, sizeof(unsigned long long int));

	while((nread = fread(buf, 1, CHUNK, fp)) > 0) {
		chunk_size = CHUNK > nread ? nread : CHUNK;
		compute_frequency_for_chunk(buf, chunk_size, frequency);
		memset(buf, CHUNK, '\0');
	}

	return frequency;
}