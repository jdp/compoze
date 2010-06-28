#include "compoze.h"

/*
 * ( -- Q )
 * Pushes an empty Quotation object to the stack.
 */
CzObject *
Quotation_new(CzState *cz)
{
	CzQuotation *self = CZ_MAKE_OBJECT(Quotation);
	self->size  = 0;
	self->cap   = 0;
	self->items = NULL;
	CZ_PUSH(self);
	return CZ_AS(Object, self);
}

/*
 * ( A Q -- Q )
 * Expects a quotation object atop the stack, and then any object type
 * underneath it. The quotation Q is popped, then the object A is popped, A
 * is appended to Q, and Q is then pushed back onto the stack.
 */
CzObject *
Quotation_append(CzState *cz, CzObject *self)
{
	CzQuotation *q = CZ_AS(Quotation, self);
	CzObject *object = CZ_POP();
	if ((q->size + 1) > q->cap) {
		q->items = (CzObject **)CZ_REALLOC(q->items, sizeof(CzObject *) * (q->cap + 1) * 2);
		q->cap = (q->cap + 1) * 2;
	}
	q->items[q->size++] = object;
	CZ_PUSH(q);
	return object;
}

CzObject *
Quotation_at(CzState *cz, CzObject *self)
{
	CzNumber *num = CZ_AS(Number, CZ_POP());
	CzObject *obj = CZ_AS(Quotation, self)->items[num->ival];
	CZ_PUSH(obj);
	return obj;
}

CzObject *
Quotation_eval(CzState *cz, CzObject *self)
{
	int i;

	if (CZ_AS(Quotation, self)->size == 0) {
		return CZ_NIL;
	}
	
	for (i = 0; i < CZ_AS(Quotation, self)->size; i++) {
		if (CZ_IS_PRIMITIVE(CZ_AS(Quotation, self)->items[i])) {
			CZ_PUSH(CZ_AS(Quotation, self)->items[i]);
		}
		else {
			switch (CZ_AS(Quotation, self)->items[i]->type) {
				case CZ_T_Symbol:
					printf("got a symbol %s, %d on stack\n", CZ_AS(Symbol, CZ_AS(Quotation, self)->items[i])->string, cz->stack->top);
					send2(CZ_AS(Quotation, self)->items[i]);
					break;
				default:
					CZ_PUSH(CZ_AS(Quotation, self)->items[i]);
					break;
			}
		}
	}
	
	return CZ_NIL;
}

void
cz_bootstrap_quotation(CzState *cz)
{
	cz_define_method(Quotation, "new", Quotation_new);
	cz_define_method(Quotation, "at", Quotation_at);
	cz_define_method(Quotation, "eval", Quotation_eval);
}
