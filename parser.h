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
	Object   *root,
	         *active,
	         *frame[32];
	int       frameptr;
} Parser;

Parser *
Parser_new(void);

int
Parser_destroy(Parser *);

Node *
Node_new(int, char *);

void
Node_destroy(Node *);

Object *
Parser_parse(Parser *, Lexer *);

void
cz_tree(Quotation *, int);

#endif /* PARSER_H */

