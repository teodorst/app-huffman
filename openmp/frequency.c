#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frequency.h"

void print_frequency(int* frequency){
	int i;
	for (i = 0 ; i < 128; i++){
		if (frequency[i] != 0)
			printf("%d ", frequency[i]);
	}

	printf("\n");
}


void compute_frequency_for_chunk_buffer(char *chunk, int chunk_size, int* frequency) {
	int i;

	for (i = 0; i < chunk_size; i ++) {
		frequency[(unsigned char)chunk[i]] ++;
	}

}

void compute_frequency_for_chunk(char *chunk, int chunk_size, int* frequency) {
	int i;

	for (i = 0; i < chunk_size; i ++) {
		frequency[(unsigned char)chunk[i]] ++;
	}

}

char *read_file(FILE *fp, long file_size) {
	size_t nread = 0;
	char *buffer = (char *)malloc(file_size * sizeof(char));
	nread = fread(buffer, 1, file_size, fp);
	if (nread != file_size) {
		return NULL;
	}
	return buffer;
}

int *compute_frequency_buffer(char *buffer, int size) {
	int* frequency = (int*) calloc(128, sizeof(int));
	compute_frequency_for_chunk_buffer(buffer, size, frequency);
	return frequency;
}

int* compute_frequency(FILE *fp) {
	size_t nread = 0;
	int chunk_size = 0;
	char buf[CHUNK];
	int* frequency = (int*) calloc(128, sizeof(int));

	memset(buf, '\0', CHUNK);
	while((nread = fread(buf, 1, CHUNK, fp)) > 0) {
		chunk_size = CHUNK > nread ? nread : CHUNK;
		compute_frequency_for_chunk(buf, chunk_size, frequency);
		memset(buf, '\0', CHUNK);
	}
	fseek(fp, 0, SEEK_SET);
	return frequency;
}