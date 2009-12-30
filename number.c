#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "object.h"
#include "number.h"

Object *
Number_new(CzState *cz, int val)
{
	Number *self  = (Number *)send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("allocate"), sizeof(Number));
	self->_vt[-1] = CZ_VTABLE(CZ_TNUMBER);
	self->ival    = val;
	self->hash    = val;
	return (Object *)self;
}

Object *
Number_hash(CzState *cz, Object *self)
{
	return (Object *)self->hash;
}

