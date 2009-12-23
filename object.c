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

VTable *symbol_list = 0;

VTable *vtable_vt;
VTable *object_vt;
VTable *symbol_vt;
VTable *number_vt;
VTable *quotation_vt;

Object *s_addMethod = 0;
Object *s_allocate  = 0;
Object *s_delegated = 0;
Object *s_lookup    = 0;

inline void *
alloc(size_t size)
{
	VTable **ppvt= (VTable **)calloc(1, sizeof(VTable *) + size);
	return (void *)(ppvt + 1);
}

Object *
VTable_lookup(VTable *self, Object *key);

Method
bind(Object *rcv, Object *msg)
{
	Method  m;
	VTable *vt = rcv->_vt[-1];
	m = ((msg == s_lookup) && (rcv == (Object *)vtable_vt))
	  ? (Method)VTable_lookup(vt, msg)
	  : (Method)send(vt, s_lookup, msg);
	return m;
}

VTable *
VTable_delegated(VTable *self)
{
	VTable *child  = (VTable *)alloc(sizeof(VTable));
	child->_vt[-1] = self ? self->_vt[-1] : 0;
	child->size    = 2;
	child->tally   = 0;
	child->keys    = (Object **)calloc(child->size, sizeof(Object *));
	child->values  = (Object **)calloc(child->size, sizeof(Object *));
	child->parent  = self;
	return child;
}

Object *
VTable_allocate(VTable *self, int payloadSize)
{
	Object *object = (Object *)alloc(payloadSize);
	object->_vt[-1] = self;
	return object;
}

Method
VTable_addMethod(VTable *self, Object *key, Method method)
{
	int i;
	for (i = 0;  i < self->tally;  ++i) {
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
VTable_lookup(VTable *self, Object *key)
{
	int i;
	for (i = 0;  i < self->tally;  ++i) {
		if (key == self->keys[i]) {
			return self->values[i];
		}
	}
	if (self->parent) {
		return send(self->parent, s_lookup, key);
	}
	fprintf(stderr, "lookup failed %p %s\n", self, ((Symbol *)key)->string);
	return 0;
}

/* Stuff for Symbols */

Object *
Symbol_new(char *string)
{
	Symbol *self = (Symbol *)alloc(sizeof(Symbol));
	self->_vt[-1] = symbol_vt;
	self->string = strdup(string);
	return (Object *)self;
}

Object *
Symbol_intern(Object *self, char *string)
{
	Object *symbol;
	int i;
	for (i = 0;  i < symbol_list->tally;  ++i) {
		symbol = symbol_list->keys[i];
		if (!strcmp(string, ((Symbol *)symbol)->string)) {
			return symbol;
		}
	}
	symbol = Symbol_new(string);
	VTable_addMethod(symbol_list, symbol, 0);
	return symbol;
}

/* Stuff for Numbers */

Object *
Number_new(int val)
{
	Number *self = (Number *)send(vtable_vt, s_allocate, sizeof(Number));
	self->_vt[-1] = number_vt;
	self->ival = val;
	return (Object *)self;
}

int
Number_value(Object *self)
{
	return ((Number *)self)->ival;
}

/* Stuff for Quotations */

Object *
Quotation_new(void)
{
	Quotation *self = (Quotation *)send(vtable_vt, s_allocate, sizeof(Quotation));
	self->_vt[-1] = quotation_vt;
	self->size = 0;
	self->cap = 0;
	return (Object *)self;
}

Object *
Quotation_append(Object *self, Object *object)
{
	Quotation *q = (Quotation *)self;
	if ((q->size + 1) > q->cap) {
		q->items = (Object **)realloc(q->items, sizeof(Object *) * (q->cap + 1) * 2);
	}
	q->items[q->size++] = object;
	return object;
}

void
bootstrap(void)
{
	vtable_vt = VTable_delegated(0);
	vtable_vt->_vt[-1] = vtable_vt;

	object_vt = VTable_delegated(0);
	object_vt->_vt[-1] = vtable_vt;
	vtable_vt->parent = object_vt;

	symbol_vt = VTable_delegated(object_vt);
	number_vt = VTable_delegated(object_vt);
	quotation_vt = VTable_delegated(object_vt);

	symbol_list = VTable_delegated(0);

	s_lookup    = Symbol_intern(0, "lookup");
	s_addMethod = Symbol_intern(0, "addMethod");
	s_allocate  = Symbol_intern(0, "allocate");
	s_delegated = Symbol_intern(0, "delegated");

	VTable_addMethod(vtable_vt, s_lookup,    (Method)VTable_lookup);
	VTable_addMethod(vtable_vt, s_addMethod, (Method)VTable_addMethod);

	send(vtable_vt, s_addMethod, s_allocate,    VTable_allocate);
	send(vtable_vt, s_addMethod, s_delegated,   VTable_delegated);
}
