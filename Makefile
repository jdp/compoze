SRC = builtin.c interpreter.c stack.c parser.c bufio.c main.c
OBJ = ${SRC:.c=.o}
LIB = -lreadline
CC = gcc
CFLAGS = -Wall -W -pedantic -O2 -g
OUT = cz

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB)
	
.c.o:
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm $(OBJ)
	rm $(OUT)
	
PHONY: clean
