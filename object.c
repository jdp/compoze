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
#include "stack.h"
#include "hash.h"
#include "table.h"
#include "object.h"
#include "number.h"

inline void *
alloc(size_t size)
{
	VTable **ppvt= (VTable **)calloc(1, sizeof(VTable *) + size);
	return (void *)(ppvt + 1);
}

VTable *
VTable_delegated(CzState *cz, VTable *parent)
{
	VTable *child  = (VTable *)alloc(sizeof(VTable));
	child->_vt[-1] = parent ? parent->_vt[-1] : 0;
	child->hash    = (size_t)(parent ? parent->_vt[-1] : 0);
	child->table   = (Table *)Table_new(cz);
	child->parent  = parent;
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
	Method m;
	
	m = (Method)Table_lookup_raw(cz, (Object *)self->table, ((Symbol *)key)->hash, key);
	if (CZ_IS_NIL(m)) {
		m = method;
		Table_insert_raw(cz, (Object *)self->table, ((Symbol *)key)->hash, key, (Object *)m);
	}
	return m;
}

Object *
VTable_lookup(CzState *cz, VTable *self, Object *key)
{
	Object *value;
	
	if (!CZ_IS_NIL(value = Table_lookup_raw(cz, (Object *)self->table, ((Symbol *)key)->hash, key))) {
		return value;
	}
	if (self->parent) {
		return send(self->parent, CZ_SYMBOL("__lookup__"), key);
	}
	return CZ_NIL;
}

Method
bind(CzState *cz, Object *rcv, Object *msg)
{
	Method m;
	VTable *vt = rcv->_vt[-1];
	
	m = ((msg == CZ_SYMBOL("__lookup__")) && (rcv == (Object *)CZ_VTABLE(CZ_TVTABLE)))
      ? (Method)VTable_lookup(0, vt, msg)
      : (Method)send(vt, CZ_SYMBOL("__lookup__"), msg);
	return m;
}

Object *
Symbol_new(CzState *cz, char *string)
{
	Object *symbol;
	Pair *pair;
	size_t hash;

	hash = djb2_hash(string, strlen(string));
	pair = (Pair *)cz->symbols->items[hash % cz->symbols->cap];
	while (!CZ_IS_NIL(pair)) {
		if (strcmp((char *)pair->key, string) == 0) {
			return pair->value;
		}
		pair = (Pair *)pair->next;
	}
	symbol = alloc(sizeof(Symbol));
	symbol->_vt[-1] = CZ_VTABLE(CZ_TSYMBOL);
	symbol->hash = hash;
	((Symbol *)symbol)->string = strdup(string);
	Table_insert_raw(cz, (Object *)cz->symbols, hash, string, symbol);
	return symbol;
}

Object *
Symbol_hash(CzState *cz, Object *self)
{
	return (Object *)self->hash;
}

Object *
Symbol_equals(CzState *cz, Object *self, Object *other)
{
	return (self == other) ? CZ_TRUE : CZ_FALSE;
}

/*
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
*/

int
bootstrap(CzState *cz)
{
	printf("booting straps...\n");
	
	cz->symbols = (Table *)Table_new(cz);
	
	CZ_VTABLE(CZ_TVTABLE)          = VTable_delegated(cz, 0);
	CZ_VTABLE(CZ_TVTABLE)->_vt[-1] = CZ_VTABLE(CZ_TVTABLE);

	CZ_VTABLE(CZ_TOBJECT)          = VTable_delegated(cz, 0);
	CZ_VTABLE(CZ_TOBJECT)->_vt[-1] = CZ_VTABLE(CZ_TVTABLE);
	CZ_VTABLE(CZ_TOBJECT)->parent  = CZ_VTABLE(CZ_TOBJECT);

	CZ_VTABLE(CZ_TSYMBOL)    = VTable_delegated(cz, CZ_VTABLE(CZ_TOBJECT));
	CZ_VTABLE(CZ_TQUOTATION) = VTable_delegated(cz, CZ_VTABLE(CZ_TOBJECT));
	CZ_VTABLE(CZ_TLIST)      = VTable_delegated(cz, CZ_VTABLE(CZ_TOBJECT));

	VTable_addMethod(cz, CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("__lookup__"), (Method)VTable_lookup);
	VTable_addMethod(cz, CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("addMethod"),  (Method)VTable_addMethod);

	send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("addMethod"), CZ_SYMBOL("allocate"),  VTable_allocate);
	send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("addMethod"), CZ_SYMBOL("delegated"), VTable_delegated);
	
	send(CZ_VTABLE(CZ_TSYMBOL), CZ_SYMBOL("addMethod"), CZ_SYMBOL("hash"), Symbol_hash);
	send(CZ_VTABLE(CZ_TSYMBOL), CZ_SYMBOL("addMethod"), CZ_SYMBOL("equals"), Symbol_equals);
	
	cz_bootstrap_number(cz);
	cz_bootstrap_table(cz);
	
	printf("straps booted.\n");
	
	return CZ_OK;
}

