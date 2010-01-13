#include "compoze.h"

/*
 * Returns an instance of a number.
 * At the moment, only integers are supported.
 * TODO: Arbitrary precision, floating point.
 */
Object *
Number_create_(CzState *cz, int val)
{
	Number *self = (Number *)send(CZ_VTABLE(CZ_T_VTABLE), CZ_SYMBOL("allocate"), sizeof(Number));
	self->vt     = CZ_VTABLE(CZ_T_NUMBER);
	self->ival   = val;
	self->hash   = val;
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
Number_equals(CzState *cz, Object *self)
{
	Object *other;
	other = CZ_POP();
	if (self->vt != other->vt) {
		CZ_PUSH(CZ_FALSE);
		return CZ_FALSE;
	}
	if (((Number *)self)->ival != ((Number *)other)->ival) {
		CZ_PUSH(CZ_FALSE);
		return CZ_FALSE;
	}
	CZ_PUSH(CZ_TRUE);
	return CZ_TRUE;
}

/*
 * Bootstraps the Number kind
 */
void
cz_bootstrap_number(CzState *cz)
{
	CZ_VTABLE(CZ_T_NUMBER) = VTable_delegated(cz, CZ_VTABLE(CZ_T_OBJECT));
	send(CZ_VTABLE(CZ_T_NUMBER), CZ_SYMBOL("add-method"), CZ_SYMBOL("hash"), Number_hash);
	send(CZ_VTABLE(CZ_T_NUMBER), CZ_SYMBOL("add-method"), CZ_SYMBOL("equals"), Number_equals);
}

