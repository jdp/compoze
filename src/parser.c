#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compoze.h"
#include "bufio.h"
#include "object.h"
#include "lexer.h"
#include "parser.h"
#include "stack.h"
#include "number.h"
#include "quotation.h"

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
	p->lineno   = 0;
	p->root     = p->active = CZ_NIL;
	p->frameptr = 0;
	for (i = 0; i < 32; i++) {
		p->frame[i] = CZ_NIL;
	}
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
	free(p);
	return CZ_OK;
}

/*
 * Builds a quotation from the token stream.
 */
Object *
Parser_parse(Parser *p, CzState *cz, Lexer *l)
{
	Object *q1, *q2, *o;
	int in_def = 0;
	int token, qdepth = 0;
	   
	q1 = Quotation_new(cz);
	Stack_push(cz->stack, q1);
	
	while ((token = Lexer_scan(l)) != T_EOF) {
		switch (token) {
		
			/* Keep track of line number */
			case T_EOL:
				p->lineno++;
				break;
				
			/* Begin quoted code */
			case T_BQUOTE:
				qdepth++;
				q1 = Quotation_new(cz);
				Stack_push(cz->stack, q1);
				break;
				
			/* End quoted code */
			case T_EQUOTE:
				qdepth--;
				q1 = Stack_pop(cz->stack);
				q2 = Stack_pop(cz->stack);
				Quotation_append(cz, q2, q1);
				Stack_push(cz->stack, q2);
				break;
				
			/* Add a word to the quotation, as a symbol */
			case T_WORD:
				o = Symbol_new(cz, l->buffer);
				q1 = Stack_pop(cz->stack);
				Quotation_append(cz, q1, o);
				Stack_push(cz->stack, q1);
				break;
				
			/* Add a number to the quotation */
			case T_NUMBER:
				o = Number_new(cz, atoi(l->buffer));
				q1 = Stack_pop(cz->stack);
				Quotation_append(cz, q1, o);
				Stack_push(cz->stack, q1);
				break;
				
			/* facepalm */
			default:
				printf("HOW COULD THIS HAVE HAPPENED, #%d?\n", token);
				return CZ_NIL;
		}
	}
	
	return Stack_peek(cz->stack);
}

/*
 * Debugging function to display a simplified node tree.
 */
void
cz_tree(CzState *cz, Quotation *q, int depth)
{
	int i;
	for (i = 0; i < q->size; i++) {
		if (q->items[i]->vt == CZ_VTABLE(CZ_TQUOTATION)) {
			printf("[ ");
			cz_tree(cz, (Quotation *)q->items[i], depth+1);
			printf("] ");
		}
		else {
			printf("W ");
		}
	}
}
	
