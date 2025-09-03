
CC = gcc
CFLAGS = -Wall -fPIC -O2

all: libpcom.so test_server test_client

libpcom.so: pcom.c pcom.h
	$(CC) $(CFLAGS) -shared -o libpcom.so pcom.c

test_server: test_server.c pcom.h libpcom.so
	$(CC) $(CFLAGS) -o test_server test_server.c -L. -lpcom

test_client: test_client.c pcom.h libpcom.so
	$(CC) $(CFLAGS) -o test_client test_client.c -L. -lpcom

clean:
	rm -f *.o *.so test_server test_client
