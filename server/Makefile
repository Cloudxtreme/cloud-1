CC=gcc
CFLAGS=-Wall -I. -std=gnu99 -pedantic -march=native -O2 -Wno-pointer-arith
LDFLAGS=-lpthread -lcrypto
OUTLIST=main.o tcpclient.o configs.o utils.o log.o tcpserver.o cloudserver.o filetransfer.o


all: cloud

cloud: $(OUTLIST)
	$(CC) $(OUTLIST) -o cloudsrv $(CFLAGS) $(LDFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

configs.o: configs.c
	$(CC) -c configs.c $(CFLAGS)

tcpclient.o: tcpclient.c
	$(CC) -c tcpclient.c $(CFLAGS)

tcpserver.o: tcpserver.c
	$(CC) -c tcpserver.c $(CFLAGS)

cloudserver.o: cloudserver.c
	$(CC) -c cloudserver.c $(CFLAGS)

log.o: log.c
	$(CC) -c log.c $(CFLAGS)

utils.o: utils.c
	$(CC) -c utils.c $(CFLAGS)

filetransfer.o: filetransfer.c
	$(CC) -c filetransfer.c $(CFLAGS)


install:

remove:

clean:
	-rm cloudsrv
	-rm *.o
	-rm *.log
