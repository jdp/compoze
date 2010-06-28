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
VTable_delegated_(CzState *cz, CzVTable *self)
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
VTable_allocate_(CzState *cz, CzVTable *self)
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
VTable_add_method_(CzState *cz, CzVTable *self, OBJ key, CzMethod method)
{
	CzMethod m;
	
	m = (CzMethod)Table_lookup_(cz, (OBJ)self->table, CZ_AS(Symbol, key)->hash, key);
	if ((OBJ)m == CZ_UNDEFINED) {
		m = method;
		Table_insert_(cz, (OBJ)self->table, CZ_AS(Symbol, key)->hash, key, (OBJ)m);
	}
	return m;
}

/*
 * Returns the vtable entry associated with the key.
 * If not found, it searches the parent vtables recursively.
 * If still not found, returns a nil object.
 */
OBJ
VTable_lookup_(CzState *cz, CzVTable *self, OBJ key)
{
	OBJ value;
	
	if ((value = Table_lookup_(cz, (OBJ)self->table, CZ_AS(Symbol, key)->hash, (OBJ)key)) != CZ_UNDEFINED) {
		return value;
	}
	if (self->parent) {
		return VTable_lookup_(cz, self->parent, key);
	}
	return CZ_UNDEFINED;
}

OBJ
bind(CzState *cz, OBJ rcv, OBJ msg)
{
	CzMethod m;
	CzVTable *vt;
	
	/*
	m = ((msg == CZ_SYMBOL("__lookup__")) && (rcv == CZ_AS(Object, CZ_VTABLE(VTable))))
      ? (CzMethod)VTable_lookup(cz, vt, msg)
      : (CzMethod)send(vt, CZ_SYMBOL("__lookup__"), msg);
    */
	if (CZ_IS_IMMEDIATE(rcv)) {
		vt = CZ_VTABLE(Object);
	}
	else {
		vt = CZ_AS(Object, rcv)->vt;
	}
    m = (CzMethod)VTable_lookup_(cz, vt, msg);
	return (OBJ)m;
}

OBJ
Symbol_intern(CzState *cz, char *string)
{
	CzSymbol *symbol;
	CzPair *pair;
	OBJ hash;
	
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
	Table_insert_(cz, (OBJ)cz->symbols, hash, (OBJ)string, (OBJ)symbol);
	return (OBJ)symbol;
}

OBJ
Object_true(CzState *cz, OBJ self)
{
	CZ_PUSH(self);
	CZ_PUSH(CZ_TRUE);
	return CZ_NIL;
}

OBJ
Object_false(CzState *cz, OBJ self)
{
	CZ_PUSH(self);
	CZ_PUSH(CZ_FALSE);
	return CZ_NIL;
}

OBJ
Object_nil(CzState *cz, OBJ self)
{
	CZ_PUSH(self);
	CZ_PUSH(CZ_NIL);
	return CZ_NIL;
}

/*
 * Since no two objects will have the same pointer, the hash values
 * of objects are just their own pointer values.
 */
OBJ 
Object_hash(CzState *cz, OBJ self)
{
	CZ_PUSH(self);
	return CZ_NIL;
}

OBJ
Object_same(CzState *cz, OBJ self)
{
	OBJ other;
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
	
	CZ_VTABLE(VTable)     = VTable_delegated_(cz, 0);
	CZ_VTABLE(VTable)->vt = CZ_VTABLE(VTable);

	CZ_VTABLE(Object)     = VTable_delegated_(cz, 0);
	CZ_VTABLE(Object)->vt = CZ_VTABLE(VTable);

	CZ_VTABLE(Symbol)    = VTable_delegated_(cz, CZ_VTABLE(Object));
	CZ_VTABLE(Word)      = VTable_delegated_(cz, CZ_VTABLE(Object));
	CZ_VTABLE(Quotation) = VTable_delegated_(cz, CZ_VTABLE(Object));
	
	cz_define_method(Object, "true", Object_true);
	cz_define_method(Object, "false", Object_false);
	cz_define_method(Object, "nil", Object_nil);
	cz_define_method(Object, "same", Object_same);
	cz_define_method(Object, "equals", Object_same);
	
	cz_bootstrap_table(cz);
	cz_bootstrap_quotation(cz);
	
	printf("straps booted.\n");
	
	return CZ_OK;
}

