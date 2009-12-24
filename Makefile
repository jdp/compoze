SRC = bufio.c lexer.c parser.c object.c stack.c main.c
OBJ = ${SRC:.c=.o}
LIB = -lreadline
CC = gcc
CFLAGS = -Wall
ifdef DEBUG
	CFLAGS += -DDEBUG
endif
OUT = cz

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB)
	
.c.o:
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm $(OBJ)
	rm $(OUT)
	
PHONY: clean
