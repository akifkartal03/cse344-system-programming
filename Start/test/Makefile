CC = gcc
CFLAGS =-pedantic-errors -std=gnu99 -g 
LIBS = -lm -lpthread -lrt
DEPS = queue.h
OBJS = queue.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: program

program: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o program