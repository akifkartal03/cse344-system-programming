CC = gcc
CFLAGS = -Wall
LIBS = -lm
DEPS = helper.h linked_list.h search.h
H_OBJ = helper.o
L_OBJ = linked_list.o
S_OBJ = search.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: myFind

myFind: $(H_OBJ) $(L_OBJ) $(S_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o myFind