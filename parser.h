#ifndef PARSER_H
#define PARSER_H

#define MAX_SIZET ((size_t)(~(size_t)0)-2)
#define DELIM "[]:;"

typedef struct cz_parser
{
	FILE *in;
	char *buffer;
	char current;
	size_t bufsize, bufused;
	int lineno;
	cz_node *nodes, *active;
	cz_node *frame[32];
	int frameptr;
} cz_parser;

#define next(p) (p->current = fgetc(p->in))
#define czP_reset_buffer(p) (p->bufused = 0)

cz_node *        czP_create_node(int, char *);
static int       czP_add_node(cz_parser *, cz_node *);
cz_parser *      czP_create(FILE *);
static void      czP_parse_comment(cz_parser *);
static void      czP_parse_signature(cz_parser *);
static void      czP_parse_number(cz_parser *);
static void      czP_parse_word(cz_parser *);
static int       czP_save(cz_parser *, int);
void             czP_parse(cz_parser *);
void             cz_tree(cz_node *, int);

#endif
