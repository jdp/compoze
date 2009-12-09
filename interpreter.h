#ifndef INTERPRETER_H
#define INTERPRETER_H

#define REGISTER(N) czI_register_word(i, #N, czW_##N)

enum { E_NOTICE, E_WARNING, E_FATAL };

typedef struct cz_interpreter cz_interpreter;

typedef void (*cz_word_fn)(cz_interpreter *i);

typedef struct cz_word
{
	char *name;
	cz_word_fn fn;
	struct cz_word *next;
} cz_word;

struct cz_interpreter
{
	cz_node *ast;
	cz_word *words;
	cz_stack *stack;
};

cz_interpreter *
czI_create(cz_node *root);

int
czI_register_word(cz_interpreter *, char *, cz_word_fn);

cz_word *
czI_get_word(cz_interpreter *, char *);

void
czI_error(cz_interpreter *, char *, ...);

void
czI_populate(cz_interpreter *);

int
czI_interpret(cz_interpreter *, cz_node *);

/* Built-in word function prototypes */

#endif
