#include "compoze.h"

/*
 * Creates and returns an empty quotation.
 */
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
Quotation_create2_(CzState *cz, OBJ first, ...)
{
	OBJ self, obj;
	va_list args;
	
	self = Quotation_create_(cz);
	obj = first;
	va_start(args, first);
	do {
		Quotation_push_(cz, CZ_AS(Quotation, self), obj);
		obj = va_arg(args, OBJ);
	} while (obj != CZ_UNWIND);
	va_end(args);
	return self;
}

OBJ
Quotation_dup_(CzState *cz, CzQuotation *self)
{
	CzQuotation *q = CZ_MAKE_OBJECT(Quotation);
	q->size = self->size;
	q->cap = self->cap;
	q->items = CZ_ALLOC_N(OBJ, q->cap);
	memcpy(q->items, self->items, sizeof(OBJ)*q->cap);
	return (OBJ)q;
}

/*
 * Appends an object to a quotation.
 */
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

/*
 * Returns the last element of a quotation.
 * Also removes that element from it.
 */
OBJ
Quotation_pop_(CzState *cz, CzQuotation *self)
{
	if (self->size <= 0) {
		printf("stack underflow\n");
		return CZ_UNDEFINED;
	}
	return self->items[--(self->size)];
}

/*
 * Get the current continuation from the continuation stack.
 * Get the next object in line from the continuation's quotation.
 * If it's the special unwind object, drop that continuation.
 * If it's an immediate, push it, otherwise send it as a message.
 */

void
Quotation_eval_(CzState *cz)
{
	CzQuotation *call, /* a 2-quote containg called quotation and its ip: [ quot ip ] */
	            *quot; /* the quotation being called */
	int ip;            /* ip of quotation being called */
	OBJ obj;           /* generic object holder */
	
	while (1) {
		
		/* our work is done here */
		if (cz->call_stack->size == 0) {
			break;
		}
		
		/* get call from call stack */
		call = CZ_AS(Quotation, cz->call_stack->items[cz->call_stack->size-1]);
		/* get the quotation and its ip from the call */
		quot = CZ_AS(Quotation, call->items[0]);
		ip = CZ_FIX2INT(call->items[1]);
		/* current element in called quotation */
		obj = quot->items[ip];
		
		/* get rid of the call if it's empty */
		if ((ip + 1) >= quot->size) {
			Quotation_pop_(cz, cz->call_stack);
		}
		
		switch(cz_proto_id(obj)) {
			
			/* if obj is a symbol, treat it as a message send */
			case CZ_T_Symbol:
				send2(obj);
				break;
				
			/* otherwise, push it to the data stack */
			default:
				CZ_PUSH(obj);
				break;
				
		}
		
		call->items[1] = CZ_INT2FIX(ip + 1);
	}

}

/*
 * Swaps the last two elements in a quotation.
 */
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

/*
 * ( I Q -- O )
 * Returns the element indexed in quotation Q by I.
 */
OBJ
Quotation_at(CzState *cz, OBJ self)
{
	OBJ num = CZ_POP();
	OBJ obj = CZ_AS(Quotation, self)->items[CZ_FIX2INT(num)];
	CZ_PUSH(obj);
	return obj;
}

/*
 * ( O Q -- Q )
 * Appends the element O to the quotation Q.
 */
OBJ
Quotation_push(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	Quotation_push_(cz, CZ_AS(Quotation, self), other);
	CZ_PUSH(self);
	return self;
}

/*
 * ( C T F -- [C? T: F] )
 * If conditional C is true, call quotation T.
 * Otherwise, call quotation F.
 */
OBJ
Quotation_if(CzState *cz, OBJ self)
{
	OBJ other, cond;
	other = CZ_POP();
	cond = CZ_POP();
	CZ_PUSH((cond == CZ_FALSE) ? other : self);
	Quotation_eval(cz, CZ_POP());
	return self;
}

OBJ
Quotation_loop(CzState *cz, OBJ self)
{
	OBJ cond;
	while (1) {
		CZ_PUSH(self);
		Quotation_eval(cz, CZ_POP());
		cond = CZ_POP();
		if (cond == CZ_FALSE) {
			break;
		}
	}
	return self;
}

OBJ
Quotation_map(CzState *cz, OBJ self)
{
	OBJ other;
	int i;
	
	other = CZ_POP();
	
	CZ_PUSH(Quotation_create_(cz));
	
	for (i = 0; i < CZ_AS(Quotation, other)->size; i++) {
		CZ_PUSH(CZ_AS(Quotation, other)->items[i]);
		Quotation_eval(cz, self);
		Quotation_eval_(cz);
		Object_swap(cz, CZ_POP());
		Quotation_push(cz, CZ_POP());
	}
	
	return self;
}

OBJ
Quotation_eval(CzState *cz, OBJ self)
{
	OBJ call;
	call = Quotation_create2_(cz, self, CZ_INT2FIX(0), CZ_UNWIND);
	Quotation_push_(cz, cz->call_stack, call);
	return call;
}

void
cz_bootstrap_quotation(CzState *cz)
{
	cz_define_method(Quotation,   "at", Quotation_at);
	cz_define_method(Quotation, "call", Quotation_eval);
	cz_define_method(Quotation, "push", Quotation_push);
	cz_define_method(Quotation,   "if", Quotation_if);
	cz_define_method(Quotation, "loop", Quotation_loop);
	cz_define_method(Quotation,  "map", Quotation_map);
}
