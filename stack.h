#ifndef STACK_H
#define STACK_H

typedef struct cz_stack
{
	int top;
	int size;
	cz_node **items;
} cz_stack;

#define czS_peek(s) (s->items[s->top])
#define czS_empty(s) (s->top == 0)

cz_stack *
czS_create(int);

int
czS_destroy(cz_stack *);

int
czS_reset(cz_stack *s);

int
czS_push(cz_stack *, cz_node *);

cz_node *
czS_pop(cz_stack *);

#endif
