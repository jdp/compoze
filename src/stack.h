#ifndef STACK_H
#define STACK_H

#define Stack_peek(s)  (s->items[s->top])
#define Stack_empty(s) (s->top == 0)

#define PUSHNUMBER(s,n) (Stack_push(s, (Object *)n))
#define POPNUMBER(s)    ((Number *)Stack_pop(s))

Stack *
Stack_new(unsigned int);

int
Stack_destroy(Stack *);

int
Stack_reset(Stack *);

int
Stack_push(Stack *, Object *);

Object *
Stack_pop(Stack *);

#endif
