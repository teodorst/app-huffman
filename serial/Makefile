C=gcc
CFLAGS=-Wall -g

build: all

all: huffman_serial.o priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -o huffman_serial $^

huffman_serial.o: huffman_serial.c priority_queue.o frequency.o huffman.o
	$(CC) $(CFLAGS) -c -o $@ huffman_serial.c


priority_queue.o: priority_queue.c priority_queue.h 
	$(CC) $(CFLAGS) -c -o $@ priority_queue.c

frequency.o: frequency.c frequency.h 
	$(CC) $(CFLAGS) -c -o $@ frequency.c

huffman.o: huffman.c huffman.h 
	$(CC) $(CFLAGS) -c -o $@ huffman.c


run: huffman_serial
	./huffman_serial compress in.in out.out

clean:
	rm *.o huffman_serial
