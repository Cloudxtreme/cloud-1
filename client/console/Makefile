CC=gcc
CFLAGS=-Wall -I. -std=gnu99 -pedantic -march=native -O2 -Wno-pointer-arith
LDFLAGS=-ljansson
LDFLAGS2=-lpthread
LDFLAGS3=-lssl -lcrypto
OUTLIST=main.o tcpclient.o configs.o utils.o log.o checkermain.o cloudclient.o checkerlife.o


all: cloud

cloud: $(OUTLIST)
	$(CC) $(OUTLIST) -o cloudclient $(CFLAGS) $(LDFLAGS) $(LDFLAGS2) $(LDFLAGS3)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

configs.o: ../configs.c
	$(CC) -c ../configs.c $(CFLAGS) $(LDFLAGS)

tcpclient.o: ../tcpclient.c
	$(CC) -c ../tcpclient.c $(CFLAGS)

log.o: ../log.c
	$(CC) -c ../log.c $(CFLAGS)

utils.o: ../utils.c
	$(CC) -c ../utils.c $(CFLAGS)

checkermain.o: ../checkermain.c
	$(CC) -c ../checkermain.c $(CFLAGS)

checkerlife.o: ../checkerlife.c
	$(CC) -c ../checkerlife.c $(CFLAGS)

cloudclient.o: ../cloudclient.c
	$(CC) -c ../cloudclient.c $(CFLAGS) $(LDFLAGS3)


install:

remove:

clean:
	-rm cloudclient
	-rm *.o
