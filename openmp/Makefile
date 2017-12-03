C=gcc
CFLAGS=-Wall -g

build: all

all: huffman_openmp_tasks.o priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -o huffman_openmp_tasks -fopenmp $^

huffman_openmp_tasks.o: huffman_openmp_tasks.c priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -c -o $@ -fopenmp huffman_openmp_tasks.c


huffman_openmp.o: huffman_OpenMP.c priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -c -o $@ -fopenmp huffman_OpenMP.c


priority_queue.o: priority_queue.c priority_queue.h 
	$(CC) $(CFLAGS) -c -o $@ -fopenmp priority_queue.c

frequency.o: frequency.c frequency.h 
	$(CC) $(CFLAGS) -c -o $@ -fopenmp frequency.c

huffman.o: huffman.c huffman.h 
	$(CC) $(CFLAGS) -c -o $@ -fopenmp huffman.c

clean:
	rm *.o huffman_openmp_tasks
