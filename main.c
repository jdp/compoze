#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compoze.h"
#include "parser.h"
#include "stack.h"

int
main(int argc, char *argv[])
{
	/* test the parser */
	cz_parser *parser = czP_create(stdin);
	czP_parse(parser);
	cz_tree(parser->nodes, 0);
	printf("\n");
	return 0;
}
