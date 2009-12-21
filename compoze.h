#ifndef COMPOZE_H
#define COMPOZE_H

enum { CZ_ERR, CZ_OK };

enum { NODE_DEFINE, NODE_QUOTE, NODE_WORD, NODE_NUMBER, NODE_BOOLEAN };

typedef struct cz_object {
	int type;
	char *value;
	int intval;
	float floatval;
	struct cz_object *next, *prev;
	struct cz_object *children;
	int refcount;
} cz_object;

typedef struct cz_object cz_node;

#endif
