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

#ifndef OBJECT_H
#define OBJECT_H

struct vtable;
struct object;
struct symbol;
struct string;

typedef struct object *(*Method)(struct object *receiver, ...);

#define CZ_OBJECT_HEADER   \
	struct vtable *_vt[0]; \
	int            refct

typedef struct vtable
{
	CZ_OBJECT_HEADER;
	int             size;
	int             tally;
	struct object **keys;
	struct object **values;
	struct vtable  *parent;
} VTable;

typedef struct object {
	CZ_OBJECT_HEADER;
} Object;

typedef struct symbol
{
	CZ_OBJECT_HEADER;
	char *string;
} Symbol;

typedef struct number
{
	CZ_OBJECT_HEADER;
	union
	{
		int ival;
		double dval;
	};
} Number;

typedef struct quotation
{
	CZ_OBJECT_HEADER;
	int             size;
	int             cap;
	struct object **items;
} Quotation;

#define send(RCV, MSG, ARGS...) ({             \
	struct object *r = (struct object *)(RCV); \
	Method         m = bind(r, (MSG));         \
	m(r, ##ARGS);                              \
})

#define incref(o) (o->refct++)
#define decref(o) (o->refct--)

#define CZ_NIL      ((Object *)0)
#define CZ_TRUE     ((Object *)1)
#define CZ_FALSE    ((Object *)2)
#define CZ_ISNIL(o) ((Object *)o == CZ_NIL)

extern struct vtable *symbol_list;

extern struct vtable *vtable_vt;
extern struct vtable *object_vt;
extern struct vtable *symbol_vt;
extern struct vtable *number_vt;

extern struct object *s_addMethod;
extern struct object *s_allocate;
extern struct object *s_delegated;
extern struct object *s_lookup;

void
bootstrap(void);

inline void *
alloc(size_t);

struct object *
VTable_lookup(struct vtable *, struct object *);

Method
bind(struct object *, struct object *);

struct vtable *
VTable_delegated(struct vtable *);

struct object *
VTable_allocate(struct vtable *, int);

Method
VTable_addMethod(struct vtable *, struct object *, Method);

struct object *
Symbol_new(char *);

struct object *
Symbol_intern(struct object *, char *);

Object *
Number_new(int val);

#endif /* OBJECT_H */
