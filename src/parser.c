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
	Parser *p = (Parser *)GC_MALLOC(sizeof(Parser));
	if (p == NULL) {
		return NULL;
	}
	p->lineno = 0;
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
	GC_FREE(p);
	return CZ_OK;
}

/*
 * Builds a quotation from the token stream.
 * The resulting quotation is left on the Compoze state's stack as
 * as the top-most item.
 */
int
Parser_parse(Parser *p, CzState *cz, Lexer *l)
{
	OBJ o;
	//int in_def = 0;
	int token, qdepth = 0;
	   
	CZ_PUSH(Quotation_create_(cz));
	
	while ((token = Lexer_scan(l)) != T_EOF) {
		switch (token) {
		
			/* Keep track of line number */
			case T_EOL:
				p->lineno++;
				break;
				
			/* Begin quoted code */
			case T_BQUOTE:
				qdepth++;
				CZ_PUSH(Quotation_create_(cz));
				break;
				
			/* End quoted code */
			case T_EQUOTE:
				qdepth--;
				Quotation_swap_(cz, cz->stack);
				Quotation_push(cz, CZ_POP());
				break;
				
			/* Add a word to the quotation, as a symbol */
			case T_WORD:
				o = Symbol_intern_(cz, l->buffer);
				CZ_PUSH(o);
				Quotation_swap_(cz, cz->stack);
				Quotation_push(cz, CZ_POP());
				break;
				
			/* Add a number to the quotation */
			case T_NUMBER:
				o = CZ_INT2FIX(atoi(l->buffer));
				CZ_PUSH(o);
				Quotation_swap_(cz, cz->stack);
				Quotation_push(cz, CZ_POP());
				break;
				
			/* facepalm */
			default:
				printf("HOW COULD THIS HAVE HAPPENED, #%d?\n", token);
				return CZ_ERR;
		}
	}
	
	return CZ_OK;
}

/*
 * Debugging function to display a simplified node tree.
 */
void
cz_tree(CzState *cz, CzQuotation *q, int depth)
{
	int i;
	for (i = 0; i < q->size; i++) {
		if (q->items[i] == CZ_NIL) {
			printf("nil ");
		}
		else if (q->items[i] == CZ_TRUE) {
			printf("true ");
		}
		else if (q->items[i] == CZ_FALSE) {
			printf("false ");
		}
		else if (CZ_IS_FIXNUM(q->items[i])) {
			printf("%d ", CZ_FIX2INT(q->items[i]));
		}
		else if (CZ_IS(String, q->items[i])) {
			printf("\"%s\" ", CZ_AS(String, q->items[i])->string);
		}
		else if (CZ_IS(Symbol, q->items[i])) {
			printf("%s ", CZ_AS(Symbol, q->items[i])->string);
		}
		else if (CZ_IS(Quotation, q->items[i])) {
			printf("[ ");
			cz_tree(cz, CZ_AS(Quotation, q->items[i]), depth+1);
			printf("] ");
		}
		else {
			printf("W:%d", cz_proto_id(q->items[i]));
		}
	}
}

