#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "compoze.h"
#include "bufio.h"
#include "parser.h"
#include "stack.h"
#include "interpreter.h"

void
repl(void)
{
	int i;
	char *prompt = "cz> ";
	char *line;
	
	cz_bufio *buf = czB_create();
	cz_parser *parser;
	cz_interpreter *interpreter;
	interpreter = czI_create();
	czI_populate(interpreter);
	
	while (1) {
		line = readline(prompt);
		if ((line == NULL) || (strcmp(line, "exit") == 0)) {
			break;
		}
		czB_reset(buf);
		buf->buffer = line;
		buf->bufsize = strlen(buf->buffer);
		parser = czP_create(buf);
		czP_parse(parser);
		czI_interpret(interpreter, parser->nodes);
		for (i = 1; i <= interpreter->stack->top; i++) {
			printf("%s ", interpreter->stack->items[i]->value);
		}
		printf("\n");
		czS_reset(interpreter->stack);
		czP_destroy(parser);
	}
	
	czB_destroy(buf);
	czI_destroy(interpreter);
}

int
main(int argc, char *argv[])
{

	repl();
	return 0;
}
