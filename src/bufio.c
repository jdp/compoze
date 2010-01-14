#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compoze.h"
#include "bufio.h"

cz_bufio *
czB_create()
{
	cz_bufio *b;
	if ((b = (cz_bufio *)GC_MALLOC(sizeof(cz_bufio))) == NULL) {
		return NULL;
	}
	b->fp = NULL;
	b->buffer = NULL;
	b->bufsize = 0;
	b->bufpos = 0;
	return b;
}

cz_bufio *
czB_create_from_file(FILE *fp)
{
	cz_bufio *b;
	if ((b = czB_create()) == NULL) {
		return NULL;
	}
	b->fp = fp;
	return b;
}

cz_bufio *
czB_create_from_string(char *str)
{
	cz_bufio *b;
	if ((b = czB_create()) == NULL) {
		return NULL;
	}
	b->buffer = strdup(str);
	b->bufsize = strlen(b->buffer);
	return b;
}

int
czB_destroy(cz_bufio *b)
{
	assert(b != NULL);
	free(b->fp);
	free(b->buffer);
	GC_FREE(b);
	return CZ_OK;
}

int
czB_getc(cz_bufio *b)
{
	if (b->fp == NULL) {
		if (b->bufpos < b->bufsize) {
			return (int)(b->buffer[b->bufpos++]);
		}
		else {
			return EOF;
		}
	}
	else {
		return fgetc(b->fp);
	}
}
