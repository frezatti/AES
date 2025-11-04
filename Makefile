CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -pedantic

OBJS = main.o aes.o aes_helpers.o

all: aes

aes: $(OBJS)
	$(CC) $(CFLAGS) -o aes $(OBJS)

main.o: main.c aes.h aes_helpers.h
	$(CC) $(CFLAGS) -c main.c

aes.o: aes.c aes.h
	$(CC) $(CFLAGS) -c aes.c

aes_helpers.o: aes_helpers.c aes_helpers.h
	$(CC) $(CFLAGS) -c aes_helpers.c

clean:
	rm -f $(OBJS) aes
