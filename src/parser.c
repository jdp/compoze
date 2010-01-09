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
	Parser *p = (Parser *)malloc(sizeof(Parser));
	if (p == NULL) {
		return NULL;
	}
	p->lineno   = 0;
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
	Object *o;
	//int in_def = 0;
	int token, qdepth = 0;
	   
	Quotation_new(cz);
	
	while ((token = Lexer_scan(l)) != T_EOF) {
		switch (token) {
		
			/* Keep track of line number */
			case T_EOL:
				p->lineno++;
				break;
				
			/* Begin quoted code */
			case T_BQUOTE:
				qdepth++;
				Quotation_new(cz);
				break;
				
			/* End quoted code */
			case T_EQUOTE:
				qdepth--;
				Stack_swap(cz->stack);
				Quotation_append(cz);
				break;
				
			/* Add a word to the quotation, as a symbol */
			case T_WORD:
				o = Symbol_intern(cz, l->buffer);
				CZ_PUSH(o);
				Stack_swap(cz->stack);
				Quotation_append(cz);
				break;
				
			/* Add a number to the quotation */
			case T_NUMBER:
				o = Number_new(cz, atoi(l->buffer));
				CZ_PUSH(o);
				Stack_swap(cz->stack);
				Quotation_append(cz);
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
	
