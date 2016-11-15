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


void compute_frequency_for_chunk(char *chunk, int chunk_size, unsigned long long int* frequency) {
	int i;
	printf("DATA: %s %d %c \n", chunk, chunk_size, chunk[0]);

	for (i = 0; i < chunk_size; i ++) {
		frequency[(unsigned char)chunk[i]] ++;
	}

}


unsigned long long int* compute_frequency(FILE *fp) {
	size_t nread = 0;
	int chunk_size = 0;
	char buf[CHUNK];
	unsigned long long int* frequency = (unsigned long long int*) calloc(128, sizeof(unsigned long long int));

	memset(buf, '\0', CHUNK);
	while((nread = fread(buf, 1, CHUNK, fp)) > 0) {
		chunk_size = CHUNK > nread ? nread : CHUNK;
		compute_frequency_for_chunk(buf, chunk_size, frequency);
		memset(buf, '\0', CHUNK);
	}
	fseek(fp, 0, SEEK_SET);
	int i; 
	for (i = 0; i < 128; i ++) {
		if (frequency[i] > 0) {
			printf("%c - %d : %d\n", i, i, (int)frequency[i]);
		}
	}
	return frequency;
}