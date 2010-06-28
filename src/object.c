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

#include "compoze.h"

/*
 * Given a parent VTable, the function returns a new VTable inheriting
 * from the provided parent VTable.
 */
CzVTable *
VTable_delegated(CzState *cz, CzVTable *self)
{
	CzVTable *child  = CZ_ALLOC(CzVTable);
	child->vt        = self ? self->vt : 0;
	child->hash      = (size_t)(self ? self->vt : 0);
	child->table     = CZ_AS(Table, Table_create_(cz));
	child->parent    = self;
	return child;
}

/*
 * Allocates space for and returns an object with a vtable specified
 * in the `self` argument.
 */
CzObject *
VTable_allocate(CzState *cz, CzVTable *self)
{
	CzObject *object = CZ_MAKE_OBJECT(Object);
	return object;
}

/*
 * Associates a key (any valid object, but usually a symbol) with a method
 *   in the specified vtable.
 * The method itself is returned.
 */
CzMethod
VTable_add_method(CzState *cz, CzVTable *self, CzObject *key, CzMethod method)
{
	CzMethod m;
	
	m = (CzMethod)Table_lookup_(cz, CZ_AS(Object, self->table), CZ_AS(Symbol, key)->hash, key);
	if (CZ_AS(Object, m) == CZ_UNDEFINED) {
		m = method;
		Table_insert_(cz, CZ_AS(Object, self->table), CZ_AS(Symbol, key)->hash, key, CZ_AS(Object, m));
	}
	return m;
}

/*
 * Returns the vtable entry associated with the key.
 * If not found, it searches the parent vtables recursively.
 * If still not found, returns a nil object.
 */
CzObject *
VTable_lookup(CzState *cz, CzVTable *self, CzObject *key)
{
	CzObject *value;
	
	if ((value = Table_lookup_(cz, CZ_AS(Object, self->table), CZ_AS(Symbol, key)->hash, key)) != CZ_UNDEFINED) {
		return value;
	}
	if (self->parent) {
		return VTable_lookup(cz, self->parent, key);
	}
	return CZ_UNDEFINED;
}

CzMethod
bind(CzState *cz, CzObject *rcv, CzObject *msg)
{
	CzMethod m;
	CzVTable *vt;
	
	printf("sending %p to %p\n", msg, rcv);
	
	/*
	m = ((msg == CZ_SYMBOL("__lookup__")) && (rcv == CZ_AS(Object, CZ_VTABLE(VTable))))
      ? (CzMethod)VTable_lookup(cz, vt, msg)
      : (CzMethod)send(vt, CZ_SYMBOL("__lookup__"), msg);
    */
	if (CZ_IS_PRIMITIVE(rcv)) {
		printf("using object vtable for %p->%p\n", msg, rcv);
		vt = CZ_VTABLE(Object);
	}
	else {
		vt = rcv->vt;
	}
    m = (CzMethod)VTable_lookup(cz, vt, msg);
	return m;
}

CzObject *
Symbol_intern(CzState *cz, char *string)
{
	CzSymbol *symbol;
	CzPair *pair;
	size_t hash;
	hash = djb2_hash(string, strlen(string));
	pair = CZ_AS(Pair, cz->symbols->items[hash % cz->symbols->cap]);
	while (!CZ_IS_NIL(pair)) {
		if (strcmp((char *)pair->key, string) == 0) {
			return pair->value;
		}
		pair = pair->next;
	}
	symbol = CZ_MAKE_OBJECT(Symbol);
	symbol->hash = hash;
	symbol->frozen = CZ_FALSE;
	symbol->string = strdup(string);
	Table_insert_(cz, CZ_OBJECT(cz->symbols), hash, string, symbol);
	return CZ_AS(Object, symbol);
}

CzObject *
Object_true(CzState *cz, CzObject *self)
{
	CZ_PUSH(self);
	CZ_PUSH(CZ_TRUE);
	return CZ_NIL;
}

CzObject *
Object_false(CzState *cz, CzObject *self)
{
	CZ_PUSH(self);
	CZ_PUSH(CZ_FALSE);
	return CZ_NIL;
}

CzObject *
Object_nil(CzState *cz, CzObject *self)
{
	CZ_PUSH(self);
	CZ_PUSH(CZ_NIL);
	return CZ_NIL;
}

/*
 * Since no two objects will have the same pointer, the hash values
 * of objects are just their own pointer values.
 */
CzObject *
Object_hash(CzState *cz, CzObject *self)
{
	CZ_PUSH(self);
	return CZ_NIL;
}

CzObject *
Object_same(CzState *cz, CzObject *self)
{
	CzObject *other;
	other = CZ_POP();
	CZ_PUSH((self == other) ? CZ_TRUE : CZ_FALSE);
	return CZ_NIL;
}

int
bootstrap(CzState *cz)
{
	printf("booting straps...\n");
	
	cz->stack = Stack_new(10);
	
	cz->symbols = CZ_AS(Table, Table_create_(cz));
	cz->strings = CZ_AS(Table, Table_create_(cz));
	
	CZ_VTABLE(VTable)     = VTable_delegated(cz, 0);
	CZ_VTABLE(VTable)->vt = CZ_VTABLE(VTable);

	CZ_VTABLE(Object)     = VTable_delegated(cz, 0);
	CZ_VTABLE(Object)->vt = CZ_VTABLE(VTable);

	CZ_VTABLE(Symbol)    = VTable_delegated(cz, CZ_VTABLE(Object));
	CZ_VTABLE(Word)      = VTable_delegated(cz, CZ_VTABLE(Object));
	CZ_VTABLE(Quotation) = VTable_delegated(cz, CZ_VTABLE(Object));
	CZ_VTABLE(List)      = VTable_delegated(cz, CZ_VTABLE(Object));
	
	cz_define_method(Object, "true", Object_true);
	cz_define_method(Object, "false", Object_false);
	cz_define_method(Object, "nil", Object_nil);
	cz_define_method(Object, "same", Object_same);
	cz_define_method(Object, "equals", Object_same);
	
	cz_bootstrap_number(cz);
	cz_bootstrap_table(cz);
	cz_bootstrap_quotation(cz);
	
	printf("straps booted.\n");
	
	return CZ_OK;
}

