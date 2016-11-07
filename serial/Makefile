C=gcc
CFLAGS=-Wall -g

build: all

all: huffman_serial.o priority_queue.o
	$(CC) $(CFLAGS) -o huffman_serial $^

huffman_serial.o: huffman_serial.c priority_queue.o
	$(CC) $(CFLAGS) -c -o $@ huffman_serial.c


priority_queue.o: priority_queue.c priority_queue.h 
	$(CC) $(CFLAGS) -c -o $@ priority_queue.c


run: huffman_serial
	./huffman_serial

clean:
	rm *.o huffman_serial