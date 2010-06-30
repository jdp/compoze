#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "compoze.h"
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
	
	GC_INIT();
	
	CzState *cz = (CzState *)GC_MALLOC(sizeof(CzState));
	bootstrap(cz);
	
	while (1) {
		line = readline(prompt);
		if ((line == NULL) || (strcmp(line, "exit") == 0)) {
			break;
		}
		buf = czB_create_from_string(line);
		lex = Lexer_new(buf);
		par = Parser_new();
		Parser_parse(par, cz, lex);
		Quotation_eval(cz, CZ_POP());
		Quotation_eval_(cz);
		printf("-- stack size: %lu --\n", cz->data_stack->size);
		cz_tree(cz, cz->data_stack, 0);
		printf("\n");
		add_history(line);
	}
	
}

int
main(int argc, char *argv[])
{
	repl();
	return 0;
}
