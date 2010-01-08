#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compoze.h"
#include "bufio.h"
#include "object.h"
#include "lexer.h"
#include "parser.h"
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
	Object *q, *o;
	int in_def = 0;
	int token;
	
	/* the first entry on the quote frame is the quote that gets
	   returned by this function */
	q = Quotation_new(cz);
	p->frame[p->frameptr] = q;
	p->active = p->frame[p->frameptr];
	
	while ((token = Lexer_scan(l)) != T_EOF) {
		switch (token) {
		
			/* Keep track of line number */
			case T_EOL:
				p->lineno++;
				break;
				
			/* Begin word definition */
			case T_BDEF:
				if (in_def) {
					printf("can't define within a definition");
					return CZ_NIL;
				}
				in_def = 1;
				break;
			
			/* End word definition */
			case T_EDEF:
				in_def = 0;
				break;
				
			/* Begin quoted code */
			case T_BQUOTE:
				p->frame[++p->frameptr] = Quotation_new(cz);
				Quotation_append(cz, p->active, p->frame[p->frameptr]);
				p->active = p->frame[p->frameptr];
				break;
				
			/* End quoted code */
			case T_EQUOTE:
				if (p->frameptr <= 0) {
					printf("not inside a quotation!\n");
					return CZ_NIL;
				}
				p->active = p->frame[--p->frameptr];
				break;
				
			/* Add a word to the quotation, as a symbol */
			case T_WORD:
				o = Symbol_new(cz, l->buffer);
				Quotation_append(cz, p->active, o);
				break;
				
			/* Add a number to the quotation */
			case T_NUMBER:
				o = Number_new(cz, atoi(l->buffer));
				Quotation_append(cz, p->active, o);
				break;
				
			/* facepalm */
			default:
				printf("HOW COULD THIS HAVE HAPPENED, #%d?\n", token);
				return CZ_NIL;
		}
	}
	
	return q;
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
	
