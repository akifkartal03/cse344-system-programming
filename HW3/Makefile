CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic-errors -std=gnu99 -g 
LIBS = -lm -lpthread -lrt
DEPS = helper.h
A_OBJ = helper.o peer.o
B_OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: peer main

peer: $(A_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

main: $(B_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


clean:
	rm -f *.o peer main