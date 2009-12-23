SRC = bufio.c lexer.c object.c stack.c main.c
OBJ = ${SRC:.c=.o}
LIB = -lreadline
CC = gcc
CFLAGS = -Wall
OUT = cz

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB)
	
.c.o:
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm $(OBJ)
	rm $(OUT)
	
PHONY: clean
