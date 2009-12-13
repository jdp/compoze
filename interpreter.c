#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "compoze.h"
#include "stack.h"
#include "interpreter.h"
#include "builtin.h"

/*
 * Creates and returns a pointer to a new interpreter.
 * Returns NULL if there was a problem creating the interpreter.
 */
cz_interpreter *
czI_create(void)
{
	cz_interpreter *i;
	
	i = (cz_interpreter *)malloc(sizeof(cz_interpreter));
	if (i == NULL) {
		return NULL;
	}
	i->ast = NULL;
	i->words = NULL;
	i->stack = czS_create(32);
	i->errorlevel = ERR_NONE;
	return i;
}

/*
 * Destroys an interpreter. Also destroys the stack associated
 *   with the interpreter.
 * Returns CZ_OK on success, and CZ_ERR on failure.
 */
int
czI_destroy(cz_interpreter *i)
{
	if (czS_destroy(i->stack) == CZ_ERR) {
		return CZ_ERR;
	}
	if (czI_unregister_words(i) == CZ_ERR) {
		return CZ_ERR;
	}
	free(i);
	return CZ_OK;
}

/*
 * Registers a Compoze word with the interpreter.
 * Returns CZ_OK on success and CZ_ERR on failure.
 */
int
czI_register_word(cz_interpreter *i, char *name, cz_word_fn f)
{
	cz_word *w;
	
	w = czI_get_word(i, name);
	if (w != NULL) {
		czI_error(i, ERR_FATAL, "word %s already defined\n", w->name);
		return CZ_ERR;
	}
	w = (cz_word *)malloc(sizeof(cz_word));
	if (w == NULL) {
		czI_error(i, ERR_FATAL, "couldn't define %s: out of memory\n", name);
		return CZ_ERR;
	}
	w->name = strdup(name);
	w->fn = f;
	w->next = i->words;
	i->words = w;
	return CZ_OK;
}

/*
 * Unregisters all the words from the interpreter, and frees memory
 *  associated with them.
 * Returns CZ_OK on success.
int
czI_unregister_words(cz_interpreter *i)
{
	cz_word *w, *next;
	w = i->words;
	while (w != NULL) {
		next = w->next;
		free(w->name);
		free(w);
		w = next;
	}
	return CZ_OK;
}

/*
 * Returns a pointer to a Compoze word.
 * Returns NULL if no such word exists.
 */
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

/*
 * Displays an error message, and sets the interpreter error level.
 */
void
czI_error(cz_interpreter *i, int level, char *fmt, ...)
{
	va_list vl;
	char buffer[512];
	
	va_start(vl, fmt);
	vsprintf(buffer, fmt, vl);
	va_end(vl);
	
	i->errorlevel = level;
	
	fputs(buffer, stderr);
}

/*
 * Populates the interpreter with built-in words.
 */
void
czI_populate(cz_interpreter *i)
{
	czI_register_word(i, "true",    czW_true);
	czI_register_word(i, "false",   czW_false);
	czI_register_word(i, "eq",      czW_eq);
	czI_register_word(i, "call",    czW_call);
	czI_register_word(i, "apply",   czW_call);
	czI_register_word(i, "print",   czW_eq);
	czI_register_word(i, "println", czW_println);
	czI_register_word(i, "dup",     czW_dup);
	czI_register_word(i, "swap",    czW_swap);
	czI_register_word(i, "dip",     czW_dip);
}

/*
 * Runs the interpreter, starting at an arbitrary node.
 * Returns CZ_OK if no errors were raised, CZ_ERR otherwise.
 */
int
czI_interpret(cz_interpreter *i, cz_node *n)
{
	cz_word *w;
	
	while ((n != NULL) && (i->errorlevel < ERR_FATAL)) {
		switch (n->type) {
			case NODE_NUMBER:
			case NODE_QUOTE:
				czS_push(i->stack, n);
				break;
			case NODE_WORD:
				w = czI_get_word(i, n->value);
				if (w == NULL) {
					czI_error(i, ERR_WARNING, "word %s undefined\n", n->value);
					break;
				}
				w->fn(i);
				break;
			default:
				czI_error(i, ERR_FATAL, "unknown AST node\n");
				break;
		}
		n = n->next;
	}
	
	return (i->errorlevel > ERR_NONE) ? CZ_ERR : CZ_OK;
}

