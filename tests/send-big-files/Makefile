CC=gcc
CFLAGS=-Wall -I. -std=gnu99 -pedantic -march=native -Wno-pointer-arith
LDFLAGS=-lssl -lcrypto
LDFLAGS2=-lpthread


all: test

test: main.o tcpclient.o filetransfer.o tcpserver.o
	$(CC) main.o tcpclient.o filetransfer.o tcpserver.o -o test $(CFLAGS) $(LDFLAGS) $(LDFLAGS2)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

tcpclient.o: ../../server/tcpclient.c
	$(CC) -c ../../server/tcpclient.c $(CFLAGS)

tcpserver.o: ../../server/tcpserver.c
	$(CC) -c ../../server/tcpserver.c $(CFLAGS) $(LDFLAGS2)

filetransfer.o: ../../server/filetransfer.c
	$(CC) -c ../../server/filetransfer.c $(CFLAGS) $(LDFLAGS)

clean:
	-rm test
	-rm *.o