#include <stdio.h>
#include <stdlib.h>
#include "compoze.h"
#include "stack.h"
#include "interpreter.h"
#include "builtin.h"

/*
 * true ( -- bool )
 * Pushes a boolean true onto the stack
 */
BUILTIN(true)
{
	cz_node *a;
	a = (cz_node *)malloc(sizeof(cz_node));
	a->type = NODE_BOOLEAN;
	a->value = "true";
	a->intval = 1;
	a->next = a->prev = a->children = NULL;
	czS_push(i->stack, a);
	return NULL;
}

/*
 * false ( -- bool )
 * Pushes a boolean false onto the stack
 */
BUILTIN(false)
{
	cz_node *a;
	a = (cz_node *)malloc(sizeof(cz_node));
	a->type = NODE_BOOLEAN;
	a->value = "false";
	a->intval = 0;
	a->next = a->prev = a->children = NULL;
	czS_push(i->stack, a);
	return NULL;
}

/*
 * eq ( 'a 'b -> bool )
 * Pushes a boolean representing the equality of the two elements atop the stack
 */
BUILTIN(eq)
{
	cz_node *a, *b;
	if (i->stack->top < 2) {
		czI_error(i, ERR_FATAL, "eq: needs 2 elements\n");
		return;
	}
	a = i->stack->items[i->stack->top];
	b = i->stack->items[i->stack->top-1];
	if (a->type != b->type) {
		czW_false(i);
	}
	else {
		switch (a->type) {
			case NODE_BOOLEAN:
				if (a->intval == b->intval) {
					czW_true(i);
				}
				else {
					czW_false(i);
				}
				break;
			default:
				czW_false(i);
		}
	}
	return NULL;
}

/*
 * call ( 'A -> )
 * Pops a quotation off of the top of the stack and executes it
 */
BUILTIN(call)
{
	cz_node *q;
	q = czS_pop(i->stack);
	if (q == NULL) {
		czI_error(i, ERR_FATAL, "call: stack empty\n");
		return;
	}
	if (q->type != NODE_QUOTE) {
		czI_error(i, ERR_FATAL, "call: tos not a quotation\n");
		return;
	}
	czI_interpret(i, q->children);
	return NULL;
}

/*
 * print ( 'a -- )
 * Pops a value off the top of the stack and prints it
 */
BUILTIN(print)
{
	cz_node *n;
	n = czS_pop(i->stack);
	if (n == NULL) {
		czI_error(i, ERR_FATAL, "print: stack empty\n");
		return;
	}
	printf("%s", n->value);
	return NULL;
}

/*
 * print ( 'a -- )
 * Pops a value off the top of the stack and prints it with a newline
 */
BUILTIN(println)
{
	cz_node *n;
	n = czS_pop(i->stack);
	if (n == NULL) {
		czI_error(i, ERR_FATAL, "print: stack empty\n");
		return;
	}
	printf("%s\n", n->value);
	return NULL;
}

/*
 * dup ( 'a -- 'a 'a )
 * Duplicates the top value on the stack
 */
BUILTIN(dup)
{
	cz_node *a;
	if (i->stack->top < 1) {
		czI_error(i, ERR_FATAL, "dup: stack empty\n");
		return;
	}
	a = czS_peek(i->stack);
	czS_push(i->stack, a);
	return NULL;
}
		

/*
 * swap ( 'a 'b -- 'b 'a )
 * Swaps the two values on top of the stack
 */
BUILTIN(swap)
{
	cz_node *a, *b;
	if (i->stack->top < 2) {
		czI_error(i, ERR_FATAL, "swap: needs 2 elements\n");
		return;
	}
	a = czS_pop(i->stack);
	b = czS_pop(i->stack);
	czS_push(i->stack, a);
	czS_push(i->stack, b);
	return NULL;
}

/*
 * dip ('A 'b ('A -> 'C) -> 'C 'b)
 * Removes second value from stack, calls quotation on top of stack,
 * and pushes the removed value back onto the stack
 */
BUILTIN(dip)
{
	cz_node *a, *q;
	if (i->stack->top < 3) {
		czI_error(i, ERR_FATAL, "dip: needs 3 elements\n");
		return;
	}
	q = czS_pop(i->stack);
	a = czS_pop(i->stack);
	czS_push(i->stack, q);
	czW_call(i);
	czS_push(i->stack, a);
	return NULL;
}
