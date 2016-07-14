CC=gcc
CFLAGS=-Wall -I. -std=gnu99 -pedantic -march=native -O2 -Wno-pointer-arith
LDFLAGS=-ljansson
LDFLAGS2=-lpthread
OUTLIST=main.o tcpclient.o configs.o utils.o log.o tcpserver.o cloudserver.o


all: meteo

meteo: $(OUTLIST)
	$(CC) $(OUTLIST) -o cloudclient $(CFLAGS) $(LDFLAGS) $(LDFLAGS2)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

configs.o: configs.c
	$(CC) -c configs.c $(CFLAGS) $(LDFLAGS)

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


install:

remove:

clean:
	-rm cloudclient
	-rm *.o
