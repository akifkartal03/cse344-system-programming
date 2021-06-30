CC = gcc
CFLAGS = -Wall -Wextra -pedantic-errors -std=gnu99 -g
LIBS = -lm
DEPS = helper.h lagrange.h
OBJS = helper.o lagrange.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: processM

processM: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o processM