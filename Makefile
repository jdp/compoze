SRC = stack.c parser.c main.c
HDR = compoze.h parser.h stack.h
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
