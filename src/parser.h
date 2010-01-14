#ifndef PARSER_H
#define PARSER_H

enum
{
	NODE_DEFINE,
	NODE_QUOTE,
	NODE_WORD,
	NODE_NUMBER
};

typedef struct cz_parser
{
	cz_bufio *in;
	int       lineno;
} Parser;

Parser *
Parser_new(void);

int
Parser_destroy(Parser *);

int
Parser_parse(Parser *, CzState *, Lexer *);

void
cz_tree(CzState *, CzQuotation *, int);

#endif /* PARSER_H */

