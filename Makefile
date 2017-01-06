CC=gcc
CFLAGS=-Wall -g

build: serial pthreads

serial: huffman_serial.o priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -o huffman_serial $^


pthreads: huffman_codification_pthreads.o frequency.o huffman.o priority_queue.o
	$(CC) $(CFLAGS) -o huffman_codification_pthreads $^


huffman_codification_pthreads.o: huffman_codification_pthreads.c frequency.o huffman.o priority_queue.o
	$(CC) $(CFLAGS) -c -o $@ huffman_codification_pthreads.c

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
