#ifndef INTERPRETER_H
#define INTERPRETER_H

#define BUILTIN(N) void czW_##N(cz_interpreter *i)

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
czI_register_word(cz_interpreter *i, char *name, cz_word_fn f);

cz_word *
czI_get_word(cz_interpreter *i, char *name);

void
czI_error(cz_interpreter *i, char *fmt, ...);

void
czI_populate(cz_interpreter *i);

int
czI_interpret(cz_interpreter *i);

/* Built-in word function prototypes */

BUILTIN(dip);

#endif
