#include "compoze.h"

/*
 * Creates and returns an empty quotation.
 */
OBJ
Quotation_create_(CzState *cz)
{
	CzQuotation *q = CZ_MAKE_OBJECT(Quotation);
	q->size  = 0;
	q->cap   = 1;
	q->items = CZ_ALLOC_N(OBJ, 1);
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
 * Removes the top object from a quotation.
 * This is unsafe.
 * Don't fuck around.
 */
OBJ
Quotation_drop_(CzState *cz, CzQuotation *self)
{
	self->size--;
	return (OBJ)self;
}

/*
 * Returns shallow duplication of the quotation.
 */
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
 * Concatenates two quotations.
 * Equal to functional composition.
 * TODO: Less naive implementation, O(N) is no good.
 */
OBJ
Quotation_concat_(CzState *cz, CzQuotation *self, CzQuotation *other)
{
	int i;
	for (i = 0; i < other->size; i++) {
		Quotation_push_(cz, self, other->items[i]);
	}
	return (OBJ)self;
}

OBJ
Quotation_unit_(CzState *cz, OBJ obj)
{
	CzQuotation *new;
	new = CZ_AS(Quotation, Quotation_create_(cz));
	Quotation_push_(cz, new, obj);
	return (OBJ)new;
}

OBJ
Quotation_cons_(CzState *cz, CzQuotation *self, OBJ obj)
{
	int i;
	CzQuotation *new;
	
	new = CZ_AS(Quotation, Quotation_create_(cz));
	Quotation_push_(cz, new, obj);
	printf("init cons_ debug: ");
	cz_tree(cz, new, 0);
	for (i = 0; i < self->size; i++) {
		Quotation_push_(cz, new, self->items[i]);
	}
	printf("final cons_ debug: ");
	cz_tree(cz, new, 0);
	return (OBJ)new;
}

/*
 * Get the current continuation from the continuation stack.
 * Get the next object in line from the continuation's quotation.
 * If it's the special unwind object, drop that continuation.
 * If it's an immediate, push it, otherwise send it as a message.
 */

CzQuotation *
Quotation_eval_step_(CzState *cz);

void
Quotation_eval_(CzState *cz)
{
	CzQuotation *call;
	
	while (1) {
		
		/* our work is done here */
		if (cz->call_stack->size == 0) {
			break;
		}
		call = Quotation_eval_step_(cz);
		call->items[1] = CZ_INT2FIX(CZ_FIX2INT(call->items[1]) + 1);
		
	}
	
}

CzQuotation *
Quotation_eval_step_(CzState *cz)
{
	CzQuotation *call,   /* a 2-quote containg called quotation, its p, and retain: [ quot n ret ] */
	            *quot,   /* the quotation being called */
				*retain; /* the retained data of the quot */
	int ip;              /* ip of quotation being called */
	OBJ obj;             /* generic object holder */
	
	/* get call from call stack */
	call = CZ_AS(Quotation, cz->call_stack->items[cz->call_stack->size-1]);
	/* get the quotation and its ip from the call */
	quot = CZ_AS(Quotation, call->items[0]);
	ip = CZ_FIX2INT(call->items[1]);
	/* current element in called quotation */
	obj = quot->items[ip];
	/* get the retain stack */
	retain = CZ_AS(Quotation, call->items[2]);
	
	#ifdef DEBUG
	printf(">> call stack <<\n");
	cz_tree(cz, cz->call_stack, 0);
	#endif
	
	if (ip < quot->size) {
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
	}
	
	if (((ip + 1) > quot->size) && (retain->size > 0)) {
		/* everything left on the retain stack should be restored */
		Quotation_concat_(cz, cz->data_stack, retain);
		retain->size = 0;
	}
	
	if ((ip + 1) > quot->size) {
		/* get rid of the call if it's empty */
		Quotation_pop_(cz, cz->call_stack);
	}
	
	#ifdef DEBUG
	printf("\n>> data stack <<\n");
	cz_tree(cz, cz->data_stack, 0);
	printf("\n");
	#endif

	return call;
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
	return Quotation_eval(cz, (cond == CZ_FALSE) ? other : self);
}

OBJ
Quotation_eval(CzState *cz, OBJ self)
{
	OBJ call;
	call = Quotation_create2_(cz, self, CZ_INT2FIX(0), Quotation_dup_(cz, cz->retain_stack), CZ_UNWIND);
	cz->retain_stack->size = 0;
	Quotation_push_(cz, cz->call_stack, call);
	return call;
}

OBJ
Quotation_dip(CzState *cz, OBJ self)
{
	OBJ call, saved;
	saved = CZ_POP();
	Quotation_push_(cz, cz->retain_stack, saved);
	Quotation_eval(cz, self);
	return self;
}

OBJ
Quotation_cons(CzState *cz, OBJ self)
{
	OBJ new, other;
	other = CZ_POP();
	new = Quotation_cons_(cz, CZ_AS(Quotation, self), other);
	CZ_PUSH(new);
	return new;
}

void
cz_bootstrap_quotation(CzState *cz)
{
	cz_define_method(Quotation,   "at", Quotation_at);
	cz_define_method(Quotation, "call", Quotation_eval);
	cz_define_method(Quotation, "push", Quotation_push);
	cz_define_method(Quotation, "cons", Quotation_cons);
	cz_define_method(Quotation,  "dip", Quotation_dip);
	cz_define_method(Quotation,   "if", Quotation_if);
}
