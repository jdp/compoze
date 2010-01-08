#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compoze.h"
#include "object.h"
#include "list.h"

Object *
List_new(CzState *cz)
{
	List *self = (List *)send(CZ_VTABLE(CZ_TVTABLE), CZ_SYMBOL("allocate"), sizeof(List));
	self->vt        = CZ_VTABLE(CZ_TLIST);
	self->size      = 0;
	self->cap       = 0;
	return (Object *)self;
}

Object *
List_at(CzState *cz, Object *self, Object *idx)
{
	return ((List *)self)->items[((Number *)idx)->ival];
}

Object *
List_append(CzState *cz, Object *self, Object *item)
{
	List *l = (List *)self;
	if ((l->size + 1) > l->cap) {
		l->items = (Object **)realloc(l->items, sizeof(Object *) * (l->cap + 1) * 2);
		l->cap = (l->cap + 1) * 2;
	}
	l->items[l->size++] = item;
	return item;
}

Object *
List_pop(CzState *cz, Object *self)
{
	List *l = (List *)self;
	if (l->size == 0) {
		return CZ_NIL;
	}
	return l->items[--(l->size)];
}

