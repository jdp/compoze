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
Quotation_append(CzState *cz)
{
	CzQuotation *q = CZ_AS(Quotation, CZ_POP());
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
Quotation_at(CzState *cz)
{
	CzQuotation *self = CZ_AS(Quotation, CZ_POP());
	CzNumber *num = CZ_AS(Number, CZ_POP());
	CzObject *obj = self->items[num->ival];
	CZ_PUSH(obj);
	return obj;
}

CzObject *
Quotation_eval(CzState *cz)
{
	int i;
	CzQuotation *self = CZ_AS(Quotation, CZ_POP());
	
	if (self->size == 0) {
		return CZ_NIL;
	}
	
	for (i = 0; i < self->size; i++) {
		if (CZ_IS_PRIMITIVE(self->items[i])) {
			CZ_PUSH(self->items[i]);
		}
		else {
			switch (self->items[i]->type) {
				case CZ_T_Symbol:
					printf("sending symbol `%s'\n", CZ_AS(Symbol, self->items[i])->string);
					send2(self->items[i]);
					break;
				default:
					printf("obj type: %d\n", self->items[i]->type);
					CZ_PUSH(self->items[i]);
					break;
			}
		}
	}
	printf("stack height: %d\n", cz->stack->top);
	
	return CZ_NIL;
}

