#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"

#define OPERATION 1
#define INPUT_FILE 2
#define OUTPUT_FILE 3
#define CODIFICATION 4
#define CHUNK 1048576


FILE* open_file(char *filename) {
	FILE* fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
        exit(EXIT_FAILURE);

    return fp;
}

void compute_frequency_for_chunk(unsigned char *chunk, int chunk_size, unsigned long long int* frequecy) {
	int i;

	for (i = 0; i < chunk_size; i ++) {
		frequecy[chunk[i]] ++;
	}

}

unsigned long long int* compute_frequency(FILE *fp) {
	size_t nread = 0;
	int chunk_size = 0;
	unsigned char buf[CHUNK];
	unsigned long long int* frequecy = (unsigned long long int*) calloc(255, sizeof(unsigned long long int));

	while((nread = fread(buf, 1, CHUNK, fp)) > 0) {
		chunk_size = CHUNK > nread ? nread : CHUNK;
		compute_frequency_for_chunk(buf, chunk_size, frequecy);
		memset(buf, CHUNK, '\0');
	}

	return frequecy;
}




int huffman_compress(char* input_filename, char* output_filename) {
	printf("compress\n");
	
	FILE* fp = open_file(input_filename);
	unsigned long long int* frequecy = compute_frequency(fp);
	heap_t *h = init_priority_queue();
	
	return 0;
}

int huffman_decompress(char* input_filename, char* output_filename, char *codification) {
	printf("decompress\n");
	return 0;
}

int main (int argc, char* argv[]) {

	if (strcmp(argv[OPERATION], "compress") == 0 && argc == 4) {
		huffman_compress(argv[INPUT_FILE], argv[OUTPUT_FILE]);
	}
	else if (strcmp(argv[OPERATION], "decompress") == 0 && argc == 5) {
		huffman_decompress(argv[INPUT_FILE], argv[OUTPUT_FILE], argv[CODIFICATION]);
	}
	else {
		printf("Bad arguments\n");
		printf("Usage huffman_serial operation[compress/decompress] input_file ouput_file [codification_file]\n");
		return 1;
	}

	return 0;
}


