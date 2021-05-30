CC = gcc
CFLAGS = -Wall
LIBS = -lm -lpthread -lrt
DEPS = list.h
OBJS = list.o test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: akif

akif: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o akif