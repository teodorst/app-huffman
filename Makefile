CC=gcc
CFLAGS=-Wall -g

build: serial pthreads_codification pthreads_decodification mpi_codification

serial: huffman_serial.o priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -o huffman_serial $^

mpi_codification: huffman_codification_mpi.o frequency.o huffman.o priority_queue.o
	mpicc -o $@ $^


pthreads_codification: huffman_codification_pthreads.o frequency.o huffman.o priority_queue.o
	$(CC) $(CFLAGS) -pthread -o huffman_codification_pthreads $^

pthreads_decodification: huffman_decodification_pthreads.o frequency.o huffman.o priority_queue.o
	$(CC) $(CFLAGS) -pthread -o huffman_decodification_pthreads $^

huffman_codification_mpi.o: huffman_codification_mpi.c frequency.o huffman.o priority_queue.o
	mpicc -c -o $@ huffman_codification_mpi.c

huffman_codification_pthreads.o: huffman_codification_pthreads.c frequency.o huffman.o priority_queue.o
	$(CC) $(CFLAGS) -c -o $@ huffman_codification_pthreads.c

huffman_decodification_pthreads.o: huffman_decodification_pthreads.c frequency.o huffman.o priority_queue.o
	$(CC) $(CFLAGS) -c -o $@ huffman_decodification_pthreads.c


huffman_serial.o: huffman_serial.c priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -c -o $@ huffman_serial.c


priority_queue.o: priority_queue.c priority_queue.h 
	$(CC) $(CFLAGS) -c -o $@ priority_queue.c

frequency.o: frequency.c frequency.h 
	$(CC) $(CFLAGS) -c -o $@ frequency.c

huffman.o: huffman.c huffman.h 
	$(CC) $(CFLAGS) -c -o $@ huffman.c


run: huffman_serial
	./huffman_serial decompress out.out in2.in dictionar

clean:
	rm *.o huffman_serial huffman_codification_pthreads
