/*
 * Generic stack interface.
 */
 
#include <stdarg.h>
#include "compoze.h"

/*
 * Creates a new stack with the specified size.
 */
CzStack *
Stack_new(unsigned int size)
{
	CzStack *s;
	s = (CzStack *)GC_MALLOC(sizeof(CzStack));
	if (s == NULL) {
		return NULL;
	}
	s->top = 0;
	s->items = (CzObject **)CZ_CALLOC(sizeof(CzObject *), size);
	if (s->items == NULL) {
		return NULL;
	}
	s->size = size;
	Stack_reset(s);
	return s;
}

/*
 * Destroys a stack.
 */
int
Stack_destroy(CzStack *s)
{
	Stack_reset(s);
	GC_FREE(s);
	return CZ_OK;
}

/*
 * Resets all the pointers in the stack to NULL. It doesn't free
 *   anything, because they're just references to nodes.
 */
int
Stack_reset(CzStack *s)
{
	int i;
	for (i = 0; i < s->size; i++) {
		s->items[i] = CZ_NIL;
	}
	s->top = 0;
	return CZ_OK;
}

/*
 * Pushes an object to the stack. The stack is automatically grown when
 *   necessary.
 */
int
Stack_push(CzStack *s, CzObject *obj)
{
	if ((s->top + 1) > s->size) {
		s->size *= 2;
		s->items = (CzObject **)CZ_REALLOC(s->items, sizeof(CzObject *) * s->size);
		if (s->items == NULL) {
			return CZ_ERR;
		}
	}
	s->items[(s->top)++] = obj;
	return CZ_OK;
}

/*
 * Pushes a variable number of objects to the stack, deferring the actual
 * work to Stack_push() so it can take advantage of automatic stack growing.
 */
int
Stack_push_bulk(CzStack *s, ...)
{
	va_list objs;
	va_start(objs, s);
	while (s) {
		if (Stack_push(s, va_arg(objs, CzObject *)) == CZ_ERR) {
			return CZ_ERR;
		}
	}
	va_end(objs);
	return CZ_OK;
}	

/*
 * Pops an object from the stack.
 */
CzObject *
Stack_pop(CzStack *s)
{
	if (s->top < 0) {
		printf("stack underflow\n");
		return CZ_NIL;
	}
	return s->items[--(s->top)];
}

int
Stack_swap(CzStack *s)
{
	CzObject *o1, *o2;
	if (s->top < 1) {
		return CZ_ERR;
	}
	o1 = Stack_pop(s);
	o2 = Stack_pop(s);
	Stack_push(s, o1);
	Stack_push(s, o2);
	return CZ_OK;
}

	
