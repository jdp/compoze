#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compoze.h"
#include "parser.h"

cz_parser *
czP_create(FILE *in)
{
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
	return p;
}

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

/* Creates and returns a new AST node with the specified type and value. */
static cz_node *
czP_create_node(int type, char *value)
{
	cz_node *node = (cz_node*)malloc(sizeof(cz_node));
	if (node == NULL) {
		return NULL;
	}
	node->type = type;
	node->value = (char *)malloc(sizeof(char) * strlen(value));
	node->next = node->prev = node->children = NULL;
	strcpy(node->value, value);
	return node;
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
		if (p->active->type == NODE_QUOTE) {
			if (p->active->children == NULL) {
				p->active->children = node;
			}
			else {
				p->active->next = node;
			}
		}
		else {
			p->active->next = node;
		}
		node->prev = p->active;
		p->active = node;
	}
	return CZ_OK;
}

static void
czP_parse_number(cz_parser *p)
{
	while ((isdigit(p->current)
	        || p->current == '.'
	        || p->current == 'e')
	       && (p->current != EOF)) {
		czP_save(p, p->current);
		next(p);
	}
	czP_save(p, '\0');
	czP_add_node(p, czP_create_node(NODE_NUMBER, p->buffer));
}

static void
czP_parse_word(cz_parser *p)
{
	while (!isspace(p->current)
	       && (strchr(DELIM, p->current) == NULL)
		   && (p->current != EOF)) {
		czP_save(p, p->current);
		next(p);
	}
	czP_save(p, '\0');
	czP_add_node(p, czP_create_node(NODE_WORD, p->buffer));
}

void
czP_parse(cz_parser *p)
{
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
			case '[':
				/* create quotation */
				p->frame[p->frameptr++] = czP_create_node(NODE_QUOTE, "#[]");
				czP_add_node(p, p->frame[p->frameptr-1]);
				next(p);
				break;
			case ']':
				/* drop down from quotation */
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

void
cz_tree(cz_node *node, int depth)
{
	int i;
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
	
