CC = gcc
CFLAGS = -Wall -ansi -Wpedantic

OBJ = bincreator

all: app run

app:
	$(CC) $(CFLAGS) -o $(OBJ) createExampleBin.c
run:
	./$(OBJ)
clean:
	rm $(OBJ)
	rm names.dat
