SRC = builtin.c interpreter.c stack.c parser.c main.c
OBJ = ${SRC:.c=.o}
CC = gcc
OUT = cz

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ)
	
.c.o:
	$(CC) -c $<
	
clean:
	rm $(OBJ)
	rm $(OUT)
	
PHONY: clean
