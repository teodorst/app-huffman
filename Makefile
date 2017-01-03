C=gcc
CFLAGS=-Wall -g

build: huffman_decodification_serial huffman_codification_serial huffman_codification_pthreads

huffman_codification_pthreads: huffman_codification_pthreads.o priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -o huffman_codification_pthreads $^

huffman_decodification_serial: huffman_decodification_serial.o priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -o huffman_decodification_serial $^

huffman_codification_serial: huffman_codification_serial.o priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -o huffman_codification_serial $^

huffman_codification_serial.o: huffman_codification_serial.c priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -c -o $@ huffman_codification_serial.c

huffman_decodification_serial.o: huffman_decodification_serial.c priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -c -o $@ huffman_decodification_serial.c

priority_queue.o: priority_queue.c priority_queue.h 
	$(CC) $(CFLAGS) -c -o $@ priority_queue.c

frequency.o: frequency.c frequency.h 
	$(CC) $(CFLAGS) -c -o $@ frequency.c

huffman.o: huffman.c huffman.h 
	$(CC) $(CFLAGS) -c -o $@ huffman.c


run: huffman_serial
	./huffman_serial decompress out.out in2.in dictionar

clean:
	rm *.o huffman_decodification_serial huffman_codification_serial
