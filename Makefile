CC = gcc
CFLAGS = -Wall -lpthread -lzmq

all: threadpool.o test easyzmq.o slave


threadpool.o: threadpool.c 
	$(CC) -c $^ $(CFLAGS)

test: threadpool.o easyzmq.o test.c
	$(CC) -o $@ $^ $(CFLAGS)

easyzmq.o: easyzmq.c
	$(CC) -c $^ $(CFLAGS)

master.o: master.c
	$(CC) -c $^ $(CFLAGS)
	
slave: easyzmq.o slave.c
	$(CC) -o $@ $^ $(CFLAGS)
	
fec: fec.c
	$(CC) -c $^ -std=c99

easyfec: easyfec.c
	$(CC) -c $^ -stdd=c99
	
common.o: common.c
	$(CC) -c $^

thrall: common.o threadpool.o easyzmq.o master.o thrall.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o test server slave common thrall *~ -rf
