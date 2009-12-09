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
	a->next = a->prev = a->children = NULL;
	czS_push(i->stack, a);
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
	a->next = a->prev = a->children = NULL;
	czS_push(i->stack, a);
}

/*
 * eq ( 'a 'b -> bool )
 * Pushes a boolean representing the equality of the two elements atop the stack
 */
BUILTIN(eq)
{
	cz_node *a, *b;
	if (i->stack->top < 2) {
		czI_error(i, "eq: needs 2 elements\n");
		return;
	}
	a = i->stack->items[i->stack->top];
	b = i->stack->items[i->stack->top-1];
	if (a->type != b->type) {
		czW_false(i);
	}
	else {
		czW_true(i);
	}
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
		czI_error(i, "call: stack empty\n");
		return;
	}
	if (q->type != NODE_QUOTE) {
		czI_error(i, "call: tos not a quotation\n");
		return;
	}
	czI_interpret(i, q->children);
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
		czI_error(i, "print: stack empty\n");
		return;
	}
	printf("%s", n->value);
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
		czI_error(i, "print: stack empty\n");
		return;
	}
	printf("%s\n", n->value);
}

/*
 * swap ( 'a 'b -- 'b 'a )
 * Swaps the two values on top of the stack
 */
BUILTIN(swap)
{
	cz_node *a, *b;
	if (i->stack->top < 2) {
		czI_error(i, "swap: needs 2 elements\n");
		return;
	}
	a = czS_pop(i->stack);
	b = czS_pop(i->stack);
	czS_push(i->stack, a);
	czS_push(i->stack, b);
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
		czI_error(i, "dip: needs 3 elements\n");
		return;
	}
	q = czS_pop(i->stack);
	a = czS_pop(i->stack);
	czS_push(i->stack, q);
	czW_call(i);
	czS_push(i->stack, a);
}
