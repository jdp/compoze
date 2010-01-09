#ifndef STACK_H
#define STACK_H

#define Stack_peek(s)  ((s)->items[(s)->top-1])
#define Stack_empty(s) ((s)->top == 0)

Stack *
Stack_new(unsigned int);

int
Stack_destroy(Stack *);

int
Stack_reset(Stack *);

int
Stack_push(Stack *, Object *);

int
Stack_push_bulk(Stack *, ...);

Object *
Stack_pop(Stack *);

int
Stack_swap(Stack *s);

#endif
