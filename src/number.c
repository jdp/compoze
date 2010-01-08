#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "object.h"
#include "number.h"

/*
 * Returns an instance of a number.
 * At the moment, only integers are supported.
 * TODO: Arbitrary precision, floating point.
 */
Object *
Number_new(CzState *cz, int val)
{
	Number *self  = (Number *)send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("allocate"), sizeof(Number));
	self->vt = CZ_VTABLE(CZ_TNUMBER);
	self->ival    = val;
	self->hash    = val;
	return (Object *)self;
}

/*
 * Returns the hash value of the number. For the moment, the hash values
 * of numbers are the same as their integer value, like in Python.
 */
Object *
Number_hash(CzState *cz, Object *self)
{
	return (Object *)self->hash;
}

/*
 * Returns whether or not two numbers are equal to each other.
 */
Object *
Number_equals(CzState *cz, Object *self, Object *other)
{
	if (self->vt != other->vt) {
		return CZ_FALSE;
	}
	if (((Number *)self)->ival != ((Number *)other)->ival) {
		return CZ_FALSE;
	}
	return CZ_TRUE;
}

/*
 * Bootstraps the Number kind
 */
void
cz_bootstrap_number(CzState *cz)
{
	CZ_VTABLE(CZ_TNUMBER) = VTable_delegated(cz, CZ_VTABLE(CZ_TOBJECT));
	/* send(CZ_VTABLE(CZ_TNUMBER), CZ_SYMBOL("addMethod"), CZ_SYMBOL("new"), Number_new); */
	send(CZ_VTABLE(CZ_TNUMBER), CZ_SYMBOL("addMethod"), CZ_SYMBOL("hash"), Number_hash);
	send(CZ_VTABLE(CZ_TNUMBER), CZ_SYMBOL("addMethod"), CZ_SYMBOL("equals"), Number_equals);
}

