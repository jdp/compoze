#ifndef PARSER_H
#define PARSER_H

#include "compoze.h"
#include "bufio.h"

#define MAX_SIZET ((size_t)(~(size_t)0)-2)
#define DELIM "[]:;"

typedef struct cz_parser
{
	cz_bufio *in;
	
	char *buffer;
	char current;
	size_t bufsize, bufused;
	
	int lineno;
	
	cz_node *nodes, *active;
	cz_node *frame[32];
	int frameptr;
} cz_parser;

int
czP_destroy(cz_parser *);

cz_node *
czP_create_node(int, char *);

void
czP_destroy_nodes(cz_node *);

cz_parser *
czP_create(cz_bufio *);

void
czP_parse(cz_parser *);

void
cz_tree(cz_node *, int);

#endif
