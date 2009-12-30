#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "object.h"
#include "quotation.h"

Object *
Quotation_new(CzState *cz)
{
	Quotation *self = (Quotation *)send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("allocate"), sizeof(Quotation));
	self->_vt[-1]   = CZ_VTABLE(CZ_TQUOTATION);
	self->size      = 0;
	self->cap       = 0;
	return (Object *)self;
}

Object *
Quotation_append(CzState *cz, Object *self, Object *object)
{
	Quotation *q = (Quotation *)self;
	if ((q->size + 1) > q->cap) {
		q->items = (Object **)realloc(q->items, sizeof(Object *) * (q->cap + 1) * 2);
		q->cap = (q->cap + 1) * 2;
	}
	q->items[q->size++] = object;
	return object;
}

Object *
Quotation_eval(CzState *cz, Object *self)
{
	int i;
	Quotation *q = (Quotation *)self;
	if (q->size == 0) {
		return CZ_NIL;
	}
	for (i = 0; i < q->size; i++) {
		if (1) {
		}
	}
	return CZ_NIL;
}
