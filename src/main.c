#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "compoze.h"
#include "object.h"
#include "stack.h"
#include "bufio.h"
#include "lexer.h"
#include "parser.h"

void
repl(void)
{
	char *prompt = "cz> ";
	char *line;
	
	cz_bufio *buf;
	Lexer *lex;
	Parser *par;
	
	CzState *cz = (CzState *)malloc(sizeof(CzState));
	bootstrap(cz);
	
	while (1) {
		line = readline(prompt);
		if ((line == NULL) || (strcmp(line, "exit") == 0)) {
			break;
		}
		buf = czB_create_from_string(line);
		lex = Lexer_new(buf);
		par = Parser_new();
		cz_tree(cz, (Quotation *)Parser_parse(par, cz, lex), 0);
		printf("\n");
		add_history(line);
		czB_destroy(buf);
		Lexer_destroy(lex);
	}
	
}

int
main(int argc, char *argv[])
{
	repl();
	return 0;
}
