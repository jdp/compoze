#ifndef COMPOZE_H
#define COMPOZE_H

enum { CZ_ERR, CZ_OK };

enum { NODE_QUOTE, NODE_WORD, NODE_NUMBER, NODE_BOOLEAN };

typedef struct cz_node {
	int type;
	char *value;
	void *morph;
	struct cz_node *next, *prev;
	struct cz_node *children;
} cz_node;

#endif
