#ifndef INTERPRETER_H
#define INTERPRETER_H

enum { ERR_NONE, ERR_NOTICE, ERR_WARNING, ERR_FATAL };

typedef struct cz_interpreter cz_interpreter;

typedef void *(*cz_word_fn)(cz_interpreter *i);

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
	int errorlevel;
};

cz_interpreter *
czI_create(void);

int
czI_register_word(cz_interpreter *, char *, cz_word_fn);

int
czI_unregister_words(cz_interpreter *);

cz_word *
czI_get_word(cz_interpreter *, char *);

void
czI_error(cz_interpreter *, int, char *, ...);

void
czI_populate(cz_interpreter *);

int
czI_interpret(cz_interpreter *, cz_node *);

#endif
