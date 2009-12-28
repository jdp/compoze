/* Copyright (c) 2007 by Ian Piumarta.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * The Software is provided "as is".  Use entirely at your own risk.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compoze.h"
#include "khash.h"
#include "object.h"

inline void *
alloc(size_t size)
{
	VTable **ppvt= (VTable **)calloc(1, sizeof(VTable *) + size);
	return (void *)(ppvt + 1);
}

VTable *
VTable_delegated(CzState *cz, VTable *self)
{
	VTable *child  = (VTable *)alloc(sizeof(VTable));
	child->_vt[-1] = self ? self->_vt[-1] : 0;
	child->hash    = (Object *)(self ? self->_vt[-1] : 0);
	child->size    = 2;
	child->tally   = 0;
	child->keys    = (Object **)calloc(child->size, sizeof(Object *));
	child->values  = (Object **)calloc(child->size, sizeof(Object *));
	child->parent  = self;
	return child;
}

Object *
VTable_allocate(CzState *cz, VTable *self, int payloadSize)
{
	Object *object = (Object *)alloc(payloadSize);
	object->_vt[-1] = self;
	return object;
}

Method
VTable_addMethod(CzState *cz, VTable *self, Object *key, Method method)
{
	int i;
	for (i = 0; i < self->tally; ++i) {
		if (key == self->keys[i]) {
			return (Method)(self->values[i] = (Object *)method);
		}
	}
	if (self->tally == self->size) {
		self->size  *= 2;
		self->keys   = (Object **)realloc(self->keys,   sizeof(Object *) * self->size);
		self->values = (Object **)realloc(self->values, sizeof(Object *) * self->size);
	}
	self->keys  [self->tally  ] = key;
	self->values[self->tally++] = (Object *)method;
	return method;
}

Object *
VTable_lookup(CzState *cz, VTable *self, Object *key)
{
	int i;
	for (i = 0; i < self->tally;  ++i) {
		if (key == self->keys[i]) {
			return self->values[i];
		}
	}
	if (self->parent) {
		return send(self->parent, CZ_SYMBOL("lookup"), key);
	}
	return CZ_NIL;
}

Method
bind(CzState *cz, Object *rcv, Object *msg)
{
	Method m;
	VTable *vt = rcv->_vt[-1];
	m = ((msg == CZ_SYMBOL("lookup")) && (rcv == (Object *)cz->vtable_vt))
      ? (Method)VTable_lookup(0, vt, msg)
      : (Method)send(vt, CZ_SYMBOL("lookup"), msg);
	return m;
}

Object *
Symbol_new(CzState *cz, char *string)
{
	Object *symbol;
	int i;
	for (i = 0;  i < cz->symbols->tally;  ++i) {
		symbol = cz->symbols->keys[i];
		if (!strcmp(string, ((Symbol *)symbol)->string)) {
			return symbol;
		}
	}
	symbol = alloc(sizeof(Symbol));
	symbol->_vt[-1] = cz->symbol_vt;
	symbol->hash = djb2_hash(string);
	((Symbol *)symbol)->string = strdup(string);
	VTable_addMethod(cz, cz->symbols, symbol, 0);
	return symbol;
}

Object *
Symbol_hash(CzState *cz, Object *self)
{
	return self->hash;
}

CzType
cz_type(Object *obj)
{
	if (CZ_IS_NIL(obj)) {
		return CZ_TNIL;
	}
	if (CZ_IS_BOOL(obj)) {
		return CZ_TBOOLEAN;
	}
	return obj->_vt[-1];
}

int
bootstrap(CzState *cz)
{
	printf("booting straps...\n");
	
	cz->vtable_vt          = VTable_delegated(cz, 0);
	cz->vtable_vt->_vt[-1] = cz->vtable_vt;

	cz->object_vt          = VTable_delegated(cz, 0);
	cz->object_vt->_vt[-1] = cz->vtable_vt;
	cz->vtable_vt->parent  = cz->object_vt;

	cz->symbol_vt    = VTable_delegated(cz, cz->object_vt);
	cz->number_vt    = VTable_delegated(cz, cz->object_vt);
	cz->quotation_vt = VTable_delegated(cz, cz->object_vt);
	cz->list_vt      = VTable_delegated(cz, cz->object_vt);
	cz->pair_vt      = VTable_delegated(cz, cz->object_vt);
	cz->table_vt     = VTable_delegated(cz, cz->object_vt);

	cz->symbols = VTable_delegated(cz, 0);

	VTable_addMethod(cz, cz->vtable_vt, CZ_SYMBOL("lookup"),    (Method)VTable_lookup);
	VTable_addMethod(cz, cz->vtable_vt, CZ_SYMBOL("addMethod"), (Method)VTable_addMethod);

	send(cz->vtable_vt, CZ_SYMBOL("addMethod"), CZ_SYMBOL("allocate"),  VTable_allocate);
	send(cz->vtable_vt, CZ_SYMBOL("addMethod"), CZ_SYMBOL("delegated"), VTable_delegated);
	
	send(cz->symbol_vt, CZ_SYMBOL("addMethod"), CZ_SYMBOL("hash"), Symbol_hash);
	
	printf("straps booted.\n");
	
	return CZ_OK;
}
