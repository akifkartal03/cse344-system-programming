CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic-errors -std=gnu99 -g
LIBS = -lm
DEPS = helper.h linked_list.h search.h
OBJS = helper.o linked_list.o search.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: myFind

myFind: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o myFind