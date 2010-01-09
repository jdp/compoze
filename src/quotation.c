#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "stack.h"
#include "object.h"
#include "quotation.h"

Object *
Quotation_new(CzState *cz)
{
	Quotation *self = (Quotation *)send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("allocate"), sizeof(Quotation));
	self->vt    = CZ_VTABLE(CZ_TQUOTATION);
	self->size  = 0;
	self->cap   = 0;
	self->items = NULL;
	return (Object *)self;
}

Object *
Quotation_append(CzState *cz, Object *self, Object *object)
{
	Quotation *q = CZ_QUOTATION(self);
	if ((q->size + 1) > q->cap) {
		q->items = (Object **)realloc(q->items, sizeof(Object *) * (q->cap + 1) * 2);
		q->cap = (q->cap + 1) * 2;
	}
	q->items[q->size++] = object;
	return object;
}

Object *
Quotation_at(CzState *cz, Quotation *self, int idx)
{
	return self->items[idx];
}

Object *
Quotation_eval(CzState *cz, Quotation *self)
{
	int i;
	Object *obj;
	
	if (self->size == 0) {
		return CZ_NIL;
	}
	
	for (i = 0; i < self->size; i++) {
		switch (self->items[i]->vt) {
			case CZ_VTABLE(CZ_TSYMBOL):
				send(Stack_pop(cz->stack), self->items[i]);
				break;
			default:
				CZ_PUSH(self->items[i]);
				break;
		}
	}
	
	return CZ_NIL;
}
