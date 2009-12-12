SRC = builtin.c interpreter.c stack.c parser.c bufio.c main.c
OBJ = ${SRC:.c=.o}
LIB = -lreadline
CC = gcc
OUT = cz

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB)
	
.c.o:
	$(CC) -c $<
	
clean:
	rm $(OBJ)
	rm $(OUT)
	
PHONY: clean
