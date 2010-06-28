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
	int i;
	char *prompt = "cz> ";
	char *line;
	
	cz_bufio *buf;
	Lexer *lex;
	Parser *par;
	OBJ o;
	
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
		for (i = 0; i < cz->stack->top; i++) {
			o = cz->stack->items[i];
			if (CZ_IS_NIL(o)) {
				printf("nil ");
			}
			else if (o == CZ_TRUE) {
				printf("true ");
			}
			else if (o == CZ_FALSE) {
				printf("false ");
			}
			else if (CZ_IS_FIXNUM(o)) {
					printf("%d ", CZ_FIX2INT(o));
			}
			else {
				printf("%d:W ", CZ_AS(Object, o)->type);
			}
		}
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
