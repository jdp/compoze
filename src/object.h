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

#define CZ_SYMBOL(s) (Symbol_intern(cz, s))

int
bootstrap(CzState *);

inline void *
alloc(size_t);

Object *
VTable_lookup(CzState *, VTable *, Object *);

VTable *
VTable_delegated(CzState *, VTable *);

Object *
VTable_allocate(CzState *, VTable *, int);

Method
VTable_add_method(CzState *, VTable *, Object *, Method);

Method
bind(CzState *, Object *, Object *);

Object *
Symbol_intern(CzState *, char *);

Object *
Symbol_hash(CzState *, Object *);

Object *
Symbol_equals(CzState *, Object *, Object *);

#endif /* OBJECT_H */
