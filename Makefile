CC = gcc 
CFLAGS = -Wall -ansi -Wpedantic

LIB_DIR = ./libs/
LIB_NAME = simpledb

OBJ = db_client

all: lib app rm
lib:
	$(CC) $(CFLAGS) -c -fPIC $(LIB_DIR)$(LIB_NAME).c
	$(CC) $(CFLAGS) -c -fPIC $(LIB_DIR)$(LIB_NAME)_file.c	
	$(CC) $(CFLAGS) -c -fPIC $(LIB_DIR)$(LIB_NAME)_lock.c	
	$(CC)  -shared -o lib_$(LIB_NAME).so $(LIB_NAME).o $(LIB_NAME)_file.o $(LIB_NAME)_lock.o
app: 
	$(CC) $(CFLAGS) -o $(OBJ) main.c -L. -l_$(LIB_NAME)    
run:
	./$(OBJ)
rm:
	rm *.o
clean:
	rm $(OBJ)
	rm lib_$(LIB_NAME).so
