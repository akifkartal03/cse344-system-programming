CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic-errors -std=gnu99 -g
LIBS = -lm -lpthread -lrt
OBJS = test.o

all: test

test: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o test