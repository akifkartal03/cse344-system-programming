CC = gcc
CFLAGS = -Wall
LIBS = -lm -lpthread -lrt
DEPS = sql_engine.h
OBJS = sql_engine.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: akif

akif: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o akif