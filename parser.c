#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bufio.h"
#include "parser.h"

/*
 * Creates a new parser from a buffered I/O stream.
 */
cz_parser *
czP_create(cz_bufio *in)
{
	int i;
	cz_parser *p = (cz_parser *)malloc(sizeof(cz_parser));
	if (p == NULL) {
		return NULL;
	}
	p->in = in;
	p->bufsize = 8096;
	p->bufused = -1;
	p->buffer = (char *)malloc(sizeof(char)*p->bufsize);
	if (p->buffer == NULL) {
		return NULL;
	}
	memset(p->buffer, 0, p->bufsize);
	p->lineno = 0;
	p->nodes = p->active = NULL;
	for (i = 0; i < 32; i++) {
		p->frame[i] = NULL;
	}
	p->frameptr = 0;
	return p;
}

/*
 * Destroys a parser.
 */
int
czP_destroy(cz_parser *p)
{
	if (p == NULL) {
		return CZ_ERR;
	}
	free(p->buffer);
	czP_destroy_nodes(p->nodes);
	free(p);
	return CZ_OK;
}

/*
 * Gets the next character available from the parser's input stream.
 */
#define next(p) (p->current = czB_getc(p->in))

/*
 * Resets the token buffer to an empty state for the next token
 * to be constructed.
 */
#define czP_reset_buffer(p) (p->bufused = 0)

/*
 * Saves a piece of the token currently being built by the parser.
 */
static int
czP_save(cz_parser *p, int c)
{
	if (p->bufused + 1 > p->bufsize) {
		size_t newsize;
		if (p->bufsize >= MAX_SIZET/2) {
			return CZ_ERR;
		}
		newsize = p->bufsize * 2;
		p->buffer = (char *)realloc(p->buffer, sizeof(char) * newsize);
		if (p->buffer == NULL) {
			return CZ_ERR;
		}
		p->bufsize = newsize;
	}
	p->buffer[p->bufused++] = (char)c;
	return CZ_OK;
}

/*
 * Creates a generic AST node.
 * The type is any of the NODE_* constants, and the value is the string
 *   representation of the value.
 */
cz_node *
czP_create_node(int type, char *value)
{
	cz_node *node = (cz_node*)malloc(sizeof(cz_node));
	if (node == NULL) {
		return NULL;
	}
	node->type = type;
	node->value = (char *)malloc(sizeof(char) * strlen(value) + 1);
	memset(node->value, '\0', strlen(value) + 1);
	node->next = node->prev = node->children = NULL;
	strcpy(node->value, value);
	return node;
}

/*
 * Recursively destroys the AST nodes in the parse tree.
 */
void
czP_destroy_nodes(cz_node *n)
{
	if (n != NULL) {
		if (n->next != NULL) {
			czP_destroy_nodes(n->next);
		}
		if (n->children != NULL) {
			czP_destroy_nodes(n->children);
		}
		free(n->value);
		if (n != NULL) {
			free(n);
		}
	}
}

/*
 * Intelligently adds a node to the current context. If the last node added
 * was a quotation, the quote's child is NULL so the node to be added is
 * added as the quotes child. If the child of the quote is not NULL, then
 * that quote's branch of the AST is filled already and the node is added
 * as a sibling node.
 */
static int
czP_add_node(cz_parser *p, cz_node *node)
{
	if (p->nodes == NULL) {
		p->nodes = node;
		p->active = node;
	}
	else {
		switch (p->active->type) {
			case NODE_DEFINE:
			case NODE_QUOTE:
				if (p->active->children == NULL) {
					p->active->children = node;
				}
				else {
					p->active->next = node;
				}
				break;
			default:
				p->active->next = node;
				break;
		}
		node->prev = p->active;
		p->active = node;
	}
	return CZ_OK;
}

static void
czP_parse_comment(cz_parser *p)
{
	while (p->current != '\r' && p->current != '\n' && p->current != EOF) {
		next(p);
	}
}

static void
czP_parse_signature(cz_parser *p)
{
	int depth;
	depth = 1;
	next(p);
	while ((depth > 0) && (p->current != EOF)) {
		switch (p->current) {
			case '(':
				depth++;
				break;
			case ')':
				depth--;
				break;
		}
		next(p);
	}
}

static void
czP_parse_number(cz_parser *p)
{
	cz_node *n;
	while ((isdigit(p->current)
	        || p->current == '.'
	        || p->current == 'e')
	       && (p->current != EOF)) {
		czP_save(p, p->current);
		next(p);
	}
	czP_save(p, '\0');
	n = czP_create_node(NODE_NUMBER, p->buffer);
	n->intval = atoi(n->value);
	n->floatval = atof(n->value);
	czP_add_node(p, n);
}

static void
czP_parse_word(cz_parser *p)
{
	cz_node *n;
	while (!isspace(p->current)
	       && (strchr(DELIM, p->current) == NULL)
		   && (p->current != EOF)) {
		czP_save(p, p->current);
		next(p);
	}
	czP_save(p, '\0');
	n = czP_create_node(NODE_WORD, p->buffer);
	n->intval = atoi(n->value);
	n->floatval = atof(n->value);
	czP_add_node(p, n);
}

/*
 * Builds a parse tree.
 */
void
czP_parse(cz_parser *p)
{
	int in_def = 0;
	
	next(p);
	for (;;) {
		switch (p->current) {
			case '\n':
			case '\r':
				p->lineno++;
			case ' ':
			case '\t':
				next(p);
				break;
			case '#':
				czP_parse_comment(p);
				next(p);
				break;
			case '(':
				czP_parse_signature(p);
				next(p);
				break;
			case ':':
				if (in_def) {
					printf("can't define within a definition");
					return;
				}
				in_def = 1;
				next(p);
				break;
			case ';':
				in_def = 0;
				next(p);
				break;
			case '[':
				/* create quotation */
				p->frame[p->frameptr++] = czP_create_node(NODE_QUOTE, "#[]");
				czP_add_node(p, p->frame[p->frameptr-1]);
				next(p);
				break;
			case ']':
				/* drop down from quotation */
				if (p->frameptr == 0) {
					printf("not inside a quotation!\n");
					return;
				}
				p->active = p->frame[--p->frameptr];
				next(p);
				break;
			case '\0':
			case EOF:
				/* null byte or eof should kill parser */
				return;
				break;
			default:
				czP_reset_buffer(p);
				if (isdigit(p->current)) {
					/* read a number, kid */
					czP_parse_number(p);
				}
				else {
					/* words start with anything but delim or number */
					czP_parse_word(p);
				}
				break;
		}
	}
}

/*
 * Debugging function to display a simplified node tree.
 */
void
cz_tree(cz_node *node, int depth)
{
	while (node != NULL) {
		if (node->type == NODE_QUOTE) {
			printf("[ ");
			cz_tree(node->children, depth+1);
			printf("] ");
		}
		else {
			printf("%s ", node->value);
		}
		node = node->next;
	}
}
	
