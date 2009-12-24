#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "compoze.h"
#include "object.h"
#include "bufio.h"
#include "lexer.h"
#include "parser.h"
#include "stack.h"

void
repl(void)
{
	char *prompt = "cz> ";
	char *line;
	
	cz_bufio *buf;
	Lexer *lex;
	Parser *par;
	
	while (1) {
		line = readline(prompt);
		if ((line == NULL) || (strcmp(line, "exit") == 0)) {
			break;
		}
		buf = czB_create_from_string(line);
		lex = Lexer_new(buf);
		par = Parser_new();
		cz_tree((Quotation *)Parser_parse(par, lex), 0);
		printf("\n");
		czB_destroy(buf);
		Lexer_destroy(lex);
	}
	
}

int
test()
{
	Stack *stack;
	Number *n;
	
	if ((stack = Stack_new(16)) == NULL) {
		printf("couldn't create stack\n");
		return 0;
	}
	else {
		printf("created stack\n");
	}
	
	n = (Number *)Number_new(5);
	PUSHNUMBER(stack, n);
	Stack_push(stack, CZ_NIL);
	n = POPNUMBER(stack);
	n = POPNUMBER(stack);
	if (n->ival == 5) {
		printf("n should be 5, but is %d\n", n->ival);
		return 0;
	}
	
	Stack_destroy(stack);
	return 1;
}

int
main(int argc, char *argv[])
{
	bootstrap();
	if (test() > 0) {
		printf("** test successful! **\n");
	}
	repl();
	return 0;
}
