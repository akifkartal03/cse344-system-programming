CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic-errors -std=gnu99 -g
LIBS = -lm -lpthread -lrt
DEPS = helper.h linked_list.h sql_engine.h queue.h
OBJ = helper.o linked_list.o sql_engine.o
S_OBJ = queue.o server.o
C_OBJ = client.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: server client

server: $(OBJ) $(S_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

client: $(OBJ) $(C_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o server client