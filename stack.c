#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "stack.h"

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
	return s;
}

int
czS_destroy(cz_stack *s)
{
	int i;
	for (i = 0; i <= s->top; i++) {
		free(s->items[i]);
	}
	free(s);
	return CZ_OK;
}

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

cz_node *
czS_pop(cz_stack *s)
{
	if (s->top <= 0) {
		return NULL;
	}
	s->top--;
	return s->items[s->top+1];
}
	
