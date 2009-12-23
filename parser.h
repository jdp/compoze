#ifndef PARSER_H
#define PARSER_H

enum {
	NODE_DEFINE,
	NODE_QUOTE,
	NODE_WORD,
	NODE_NUMBER
};

typedef struct cz_node {
	int             type;
	char           *value;
	struct cz_node *next;
	struct cz_node *prev;
	struct cz_node *children;
} Node;

typedef struct Parser
{
	cz_bufio *in;
	int       lineno;
	Node     *nodes;
	Node     *active;
	Node     *frame[32];
	int       frameptr;
} Parser;

Parser *
Parser_new(void);

int
Parser_destroy(Parser *);

Node *
Parser_create_node(int, char *);

void
Parser_destroy_nodes(Node *);

int
Parser_parse(Parser *, Lexer *);

void
cz_tree(Node *, int);

#endif /* PARSER_H */
