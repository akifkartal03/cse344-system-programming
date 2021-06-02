CC = gcc
CFLAGS = -Wall
LIBS = -lm -lpthread -lrt
DEPS = linked_list.h sql_engine.h
OBJS = linked_list.o sql_engine.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: akif

akif: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o akif