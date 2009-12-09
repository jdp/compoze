#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "compoze.h"
#include "stack.h"
#include "interpreter.h"
#include "builtin.h"

cz_interpreter *
czI_create(cz_node *root)
{
	cz_interpreter *i;
	
	i = (cz_interpreter *)malloc(sizeof(cz_interpreter));
	if (i == NULL) {
		return NULL;
	}
	i->ast = root;
	i->stack = czS_create(32);
	return i;
}

int
czI_register_word(cz_interpreter *i, char *name, cz_word_fn f)
{
	cz_word *w;
	
	w = czI_get_word(i, name);
	if (w != NULL) {
		czI_error(i, "word %s already defined\n", w->name);
		return CZ_ERR;
	}
	w = (cz_word *)malloc(sizeof(cz_word));
	if (w == NULL) {
		czI_error(i, "couldn't define %s: out of memory\n", name);
		return CZ_ERR;
	}
	w->name = strdup(name);
	w->fn = f;
	w->next = i->words;
	i->words = w;
	return CZ_OK;
}

cz_word *
czI_get_word(cz_interpreter *i, char *name)
{
	cz_word *w;
	w = i->words;
	while (w != NULL) {
		if (strcmp(w->name, name) == 0) {
			return w;
		}
		w = w->next;
	}
	return NULL;
}

void
czI_error(cz_interpreter *i, char *fmt, ...)
{
	va_list vl;
	char buffer[512];
	
	va_start(vl, fmt);
	vsprintf(buffer, fmt, vl);
	va_end(vl);
	
	fputs(buffer, stderr);
}

void
czI_populate(cz_interpreter *i)
{
	REGISTER(true);
	REGISTER(false);
	REGISTER(eq);
	REGISTER(call);
	REGISTER(print);
	REGISTER(println);
	REGISTER(swap);
	REGISTER(dip);
};

int
czI_interpret(cz_interpreter *i, cz_node *n)
{
	cz_word *w;
	int fatal;
	
	fatal = 0;
	while ((n != NULL) && !fatal) {
		switch (n->type) {
			case NODE_NUMBER:
			case NODE_QUOTE:
				czS_push(i->stack, n);
				break;
			case NODE_WORD:
				w = czI_get_word(i, n->value);
				if (w == NULL) {
					czI_error(i, "word %s undefined\n", n->value);
					fatal = 1;
					break;
				}
				w->fn(i);
				break;
			default:
				printf("unknown ast node\n");
				fatal = 1;
				break;
		}
		n = n->next;
	}
	return fatal ? CZ_ERR : CZ_OK;
}
					
