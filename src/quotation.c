#include "compoze.h"

OBJ
Quotation_create_(CzState *cz)
{
	CzQuotation *q = CZ_MAKE_OBJECT(Quotation);
	q->size  = 0;
	q->cap   = 0;
	q->items = NULL;
	return (OBJ)q;
}

OBJ
Quotation_push_(CzState *cz, CzQuotation *self, OBJ object)
{
	if ((self->size + 1) > self->cap) {
		self->items = (OBJ *)CZ_REALLOC(self->items, sizeof(OBJ) * (self->cap + 1) * 2);
		self->cap = (self->cap + 1) * 2;
	}
	self->items[self->size++] = object;
	return (OBJ)self;
}

OBJ
Quotation_pop_(CzState *cz, CzQuotation *self)
{
	if (self->size <= 0) {
		printf("stack underflow\n");
		return CZ_UNDEFINED;
	}
	return self->items[--(self->size)];
}

OBJ
Quotation_swap_(CzState *cz, CzQuotation *self)
{
	OBJ o1, o2;
	if (self->size < 2) {
		return CZ_UNDEFINED;
	}
	o1 = Quotation_pop_(cz, self);
	o2 = Quotation_pop_(cz, self);
	Quotation_push_(cz, self, o1);
	Quotation_push_(cz, self, o2);
	return (OBJ)self;
}

OBJ
Quotation_at(CzState *cz, OBJ self)
{
	OBJ num = CZ_POP();
	OBJ obj = CZ_AS(Quotation, self)->items[CZ_FIX2INT(num)];
	CZ_PUSH(obj);
	return obj;
}

OBJ
Quotation_push(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	Quotation_push_(cz, CZ_AS(Quotation, self), other);
	CZ_PUSH(self);
	return self;
}

OBJ
Quotation_eval(CzState *cz, OBJ self)
{
	int i;

	if (CZ_AS(Quotation, self)->size == 0) {
		return CZ_NIL;
	}
	
	for (i = 0; i < CZ_AS(Quotation, self)->size; i++) {
		if (CZ_IS_IMMEDIATE(CZ_AS(Quotation, self)->items[i])) {
			CZ_PUSH(CZ_AS(Quotation, self)->items[i]);
		}
		else {
			switch (CZ_AS(Object, CZ_AS(Quotation, self)->items[i])->type) {
				case CZ_T_Symbol:
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
	cz_define_method(Quotation, "at", Quotation_at);
	cz_define_method(Quotation, "i", Quotation_eval);
	cz_define_method(Quotation, "push", Quotation_push);
}
