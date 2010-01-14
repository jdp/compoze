#include "compoze.h"

/*
 * Returns an instance of a number.
 * At the moment, only integers are supported.
 * TODO: Arbitrary precision, floating point.
 */
CzObject *
Number_create_(CzState *cz, int val)
{
	CzNumber *self = CZ_MAKE_OBJECT(Number);
	self->ival   = val;
	self->hash   = val;
	return CZ_AS(Object, self);
}

/*
 * Returns the hash value of the number. For the moment, the hash values
 * of numbers are the same as their integer value, like in Python.
 */
CzObject *
Number_hash(CzState *cz, CzObject *self)
{
	CZ_PUSH(CZ_AS(Object, self->hash));
	return CZ_NIL;
}

/*
 * Returns whether or not two numbers are equal to each other.
 */
CzObject *
Number_equals(CzState *cz, CzObject *self)
{
	CzObject *other;
	other = CZ_POP();
	if (self->vt != other->vt) {
		CZ_PUSH(CZ_FALSE);
		return CZ_FALSE;
	}
	if (CZ_AS(Number, self)->ival != CZ_AS(Number, other)->ival) {
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
	CZ_VTABLE(Number) = VTable_delegated(cz, CZ_VTABLE(Object));
	VTable_add_method(cz, CZ_VTABLE(Number), CZ_SYMBOL("hash"), (CzMethod)Number_hash);
	VTable_add_method(cz, CZ_VTABLE(Number), CZ_SYMBOL("equals"), (CzMethod)Number_equals);
}

