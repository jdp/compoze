#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "stack.h"
#include "object.h"
#include "quotation.h"

/*
 * ( -- Q )
 * Pushes an empty Quotation object to the stack.
 */
Object *
Quotation_new(CzState *cz)
{
	Quotation *self = CZ_QUOTATION(send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("allocate"), sizeof(Quotation)));
	self->vt    = CZ_VTABLE(CZ_TQUOTATION);
	self->size  = 0;
	self->cap   = 0;
	self->items = NULL;
	CZ_PUSH(self);
	return CZ_OBJECT(self);
}

/*
 * ( A Q -- Q )
 * Expects a quotation object atop the stack, and then any object type
 * underneath it. The quotation Q is popped, then the object A is popped, A
 * is appended to Q, and Q is then pushed back onto the stack.
 */
Object *
Quotation_append(CzState *cz)
{
	Quotation *q = CZ_QUOTATION(CZ_POP());
	Object *object = CZ_POP();
	if ((q->size + 1) > q->cap) {
		q->items = (Object **)realloc(q->items, sizeof(Object *) * (q->cap + 1) * 2);
		q->cap = (q->cap + 1) * 2;
	}
	q->items[q->size++] = object;
	CZ_PUSH(q);
	return object;
}

Object *
Quotation_at(CzState *cz)
{
	Quotation *self = CZ_QUOTATION(CZ_POP());
	Number *num = CZ_NUMBER(CZ_POP());
	Object *obj = self->items[num->ival];
	CZ_PUSH(obj);
	return obj;
}

Object *
Quotation_eval(CzState *cz)
{
	int i;
	Quotation *self = CZ_QUOTATION(CZ_POP());
	
	if (self->size == 0) {
		return CZ_NIL;
	}
	
	for (i = 0; i < self->size; i++) {
		switch (CZ_VTYPE_ID(self->items[i]->vt)) {
			case CZ_TSYMBOL:
				apisend(Stack_pop(cz->stack), self->items[i]);
				break;
			default:
				CZ_PUSH(self->items[i]);
				break;
		}
	}
	
	return CZ_NIL;
}
