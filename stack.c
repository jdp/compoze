/*
 * Generic stack interface.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "stack.h"

/*
 * Creates a new stack with the specified size.
 */
cz_stack *
czS_create(int size)
{
	cz_stack *s;
	s = (cz_stack *)malloc(sizeof(cz_stack));
	if (s == NULL) {
		return NULL;
	}
	s->top = 0;
	s->items = (cz_node **)malloc(sizeof(cz_node *)*size);
	if (s->items == NULL) {
		return NULL;
	}
	s->size = size;
	czS_reset(s);
	return s;
}

/*
 * Destroys a stack.
 */
int
czS_destroy(cz_stack *s)
{
	czS_reset(s);
	free(s->items);
	free(s);
	return CZ_OK;
}

/*
 * Resets all the pointers in the stack to NULL. It doesn't free
 *   anything, because they're just references to nodes.
 */
int
czS_reset(cz_stack *s)
{
	int i;
	for (i = 0; i < s->size; i++) {
		s->items[i] = NULL;
	}
	s->top = 0;
	return CZ_OK;
}

/*
 * czS_empty(cz_stack *s)
 * Returns whether or not the stack is empty.
 * Implemented as a macro in stack.h.
 */

/*
 * czS_peek(cz_stack *s)
 * Returns the value at the top of the stack without removing it.
 * Implemented as a macro in stack.h.
 */

/*
 * Pushes a node to the stack. The stack is automatically grown when
 *   necessary.
 */
int
czS_push(cz_stack *s, cz_node *n)
{
	if (s->top + 1 > s->size) {
		int newsize;
		newsize = s->size * 2;
		s->items = (cz_node **)realloc(s->items, sizeof(cz_node *)*newsize);
		if (s->items == NULL) {
			return CZ_ERR;
		}
	}
	s->items[++s->top] = n;
	return CZ_OK;
}

/*
 * Pops a node from the stack.
 */
cz_node *
czS_pop(cz_stack *s)
{
	if (s->top <= 0) {
		return NULL;
	}
	s->top--;
	return s->items[s->top+1];
}
	
