SRC = interpreter.c stack.c parser.c main.c
HDR = compoze.h interpreter.h stack.h parser.h
OBJ = ${SRC:.c=.o}
CC = gcc
OUT = cz

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ)
	
.c.o: $(HDR)
	$(CC) -c $<
	
clean:
	rm $(OBJ)
	rm $(OUT)
	
PHONY: clean
