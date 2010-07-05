#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "compoze.h"
#include "bufio.h"
#include "lexer.h"
#include "parser.h"

int
cz_do_file(CzState *cz, char *filename)
{
	FILE *fp;
	cz_bufio *buf;
	Lexer *lex;
	Parser *par;
	
	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "couldn't open file `%s'\n", filename);
		return CZ_ERR;
	}
	
	buf = czB_create_from_file(fp);
	lex = Lexer_new(buf);
	par = Parser_new();
	Parser_parse(par, cz, lex);
	
	fclose(fp);
	
	Quotation_call(cz, CZ_POP());
	Quotation_eval_(cz);
	
	return CZ_OK;
}

void
repl(char *prelude_filename)
{
	char *prompt = "cz> ";
	char *line;
	
	cz_bufio *buf;
	Lexer *lex;
	Parser *par;
	
	GC_INIT();
	
	CzState *cz = (CzState *)GC_MALLOC(sizeof(CzState));
	bootstrap(cz);
	
	if (prelude_filename != NULL) {
		if (cz_do_file(cz, prelude_filename) == CZ_ERR) {
			exit(1);
		}
	}
	
	while (1) {
		line = readline(prompt);
		if ((line == NULL) || (strcmp(line, "exit") == 0)) {
			break;
		}
		buf = czB_create_from_string(line);
		lex = Lexer_new(buf);
		par = Parser_new();
		Parser_parse(par, cz, lex);
		Quotation_call(cz, CZ_POP());
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
	int ch;
	char *prelude, *filename;
	
	static struct option longopts[] = {
		{ "prelude", required_argument, NULL, 'p'}
	};
	
	while ((ch = getopt_long(argc, argv, "p:", longopts, NULL)) != -1) {
		switch (ch) {
			case 'p':
				prelude = strdup(optarg);
				break;
		}
	}
	
	argc -= optind;
	argv += optind;
	
	repl(prelude);
	return 0;
}
