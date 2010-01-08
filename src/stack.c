/*
 * Generic stack interface.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "compoze.h"
#include "object.h"
#include "stack.h"

/*
 * Creates a new stack with the specified size.
 */
Stack *
Stack_new(unsigned int size)
{
	Stack *s;
	s = (Stack *)malloc(sizeof(Stack));
	if (s == NULL) {
		return NULL;
	}
	s->top = 0;
	s->items = (Object **)malloc(sizeof(Object *) * size);
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
Stack_destroy(Stack *s)
{
	Stack_reset(s);
	free(s->items);
	free(s);
	return CZ_OK;
}

/*
 * Resets all the pointers in the stack to NULL. It doesn't free
 *   anything, because they're just references to nodes.
 */
int
Stack_reset(Stack *s)
{
	int i;
	for (i = 0; i < s->size; i++) {
		s->items[i] = CZ_NIL;
	}
	s->top = 0;
	return CZ_OK;
}

/*
 * Stack_empty(Stack *s)
 * Returns whether or not the stack is empty.
 * Implemented as a macro in stack.h.
 */

/*
 * Stack_peek(Stack *s)
 * Returns the value at the top of the stack without removing it.
 * Implemented as a macro in stack.h.
 */

/*
 * Pushes an object to the stack. The stack is automatically grown when
 *   necessary.
 */
int
Stack_push(Stack *s, Object *obj)
{
	if ((s->top + 1) > s->size) {
		s->items = (Object **)realloc(s->items, sizeof(Object *) * s->size * 2);
		if (s->items == NULL) {
			return CZ_ERR;
		}
	}
	s->items[++(s->top)] = obj;
	return CZ_OK;
}

/*
 * Pushes a variable number of objects to the stack, deferring the actual
 * work to Stack_push() so it can take advantage of automatic stack growing.
 */
int
Stack_push_bulk(Stack *s, ...)
{
	va_list objs;
	va_start(objs, s);
	while (s) {
		if (Stack_push(s, va_arg(objs, Object *)) == CZ_ERR) {
			return CZ_ERR;
		}
	}
	va_end(objs);
	return CZ_OK;
}	

/*
 * Pops an object from the stack.
 */
Object *
Stack_pop(Stack *s)
{
	if (s->top <= 0) {
		return CZ_NIL;
	}
	return s->items[(s->top)--];
}
	
