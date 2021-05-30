CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic-errors -std=gnu99 -g
LIBS = -lm -lpthread -lrt
DEPS = helper.h
OBJS = helper.o server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: server

server: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o server