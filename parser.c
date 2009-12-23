#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compoze.h"
#include "bufio.h"
#include "lexer.h"
#include "parser.h"

/*
 * Creates a new parser.
 */
Parser *
Parser_new(void)
{
	int i;
	Parser *p = (Parser *)malloc(sizeof(Parser));
	if (p == NULL) {
		return NULL;
	}
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
Parser_destroy(Parser *p)
{
	if (p == NULL) {
		return CZ_ERR;
	}
	Parser_destroy_nodes(p->nodes);
	free(p);
	return CZ_OK;
}

/*
 * Creates a generic AST node.
 * The type is any of the NODE_* constants, and the value is the string
 *   representation of the value.
 */
Node *
Parser_create_node(int type, char *value)
{
	Node *node = (Node *)malloc(sizeof(Node));
	if (node == NULL) {
		return NULL;
	}
	node->type = type;
	node->value = strdup(value);
	node->next = node->prev = node->children = NULL;
	return node;
}

/*
 * Recursively destroys the AST nodes in the parse tree.
 */
void
Parser_destroy_nodes(Node *n)
{
	if (n != NULL) {
		if (n->next != NULL) {
			Parser_destroy_nodes(n->next);
		}
		if (n->children != NULL) {
			Parser_destroy_nodes(n->children);
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
Parser_add_node(Parser *p, Node *node)
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


/*
 * Builds a parse tree.
 */
int
Parser_parse(Parser *p, Lexer *l)
{
	int in_def = 0;
	int token;
	Node *node;
	
	while ((token = Lexer_scan(l)) != T_EOF) {
		switch (token) {
			case T_EOL:
				p->lineno++;
				break;
			case T_BDEF:
				if (in_def) {
					printf("can't define within a definition");
					return CZ_ERR;
				}
				in_def = 1;
				break;
			case T_EDEF:
				in_def = 0;
				break;
			case T_BQUOTE:
				/* create quotation */
				p->frame[p->frameptr] = Parser_create_node(NODE_QUOTE, "#[]");
				Parser_add_node(p, p->frame[p->frameptr++]);
				break;
			case T_EQUOTE:
				/* drop down from quotation */
				if (p->frameptr == 0) {
					printf("not inside a quotation!\n");
					return CZ_ERR;
				}
				p->active = p->frame[--p->frameptr];
				break;
			case T_WORD:
				node = Parser_create_node(NODE_WORD, l->buffer);
				Parser_add_node(p, node);
				break;
			case T_NUMBER:
				node = Parser_create_node(NODE_NUMBER, l->buffer);
				Parser_add_node(p, node);
				break;
			default:
				printf("HOW COULD THIS HAVE HAPPENED, #%d?\n", token);
				return CZ_ERR;
				break;
		}
	}
	
	return CZ_OK;
}

/*
 * Debugging function to display a simplified node tree.
 */
void
cz_tree(Node *node, int depth)
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
	
