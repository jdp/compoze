SRC = bufio.c \
      lexer.c \
      parser.c \
      object.c \
      number.c \
      string.c \
      table.c \
      quotation.c \
      main.c
OBJ = ${SRC:.c=.o}
LIB = -lreadline `pkg-config --libs bdw-gc`
CC = clang
CFLAGS = -Wall `pkg-config --cflags bdw-gc`
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
