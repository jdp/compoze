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

extern struct cz_vtable *symbol_list;

extern struct cz_vtable *vtable_vt;
extern struct cz_vtable *object_vt;
extern struct cz_vtable *symbol_vt;
extern struct cz_vtable *number_vt;
extern struct cz_vtable *quotation_vt;

extern struct cz_object *s_addMethod;
extern struct cz_object *s_allocate;
extern struct cz_object *s_delegated;
extern struct cz_object *s_lookup;

void
bootstrap(void);

inline void *
alloc(size_t);

struct cz_object *
VTable_lookup(struct cz_vtable *, struct cz_object *);

Method
bind(struct cz_object *, struct cz_object *);

struct cz_vtable *
VTable_delegated(struct cz_vtable *);

struct cz_object *
VTable_allocate(struct cz_vtable *, int);

Method
VTable_addMethod(struct cz_vtable *, struct cz_object *, Method);

struct cz_object *
Symbol_new(char *);

struct cz_object *
Symbol_intern(struct cz_object *, char *);

Object *
Number_new(int val);

Object *
Quotation_new(void);

Object *
Quotation_append(Object *, Object *);

#endif /* OBJECT_H */
