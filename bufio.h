#ifndef BUFIO_H
#define BUFIO_H

typedef struct cz_bufio
{
	FILE *fp;
	char *buffer;
	int bufsize, bufpos;
} cz_bufio;

#define czB_reset(b) (b->bufpos = 0)

cz_bufio *
czB_create();

cz_bufio *
czB_create_from_file(FILE *);

cz_bufio *
czB_create_from_string(char *);

int
czB_destroy(cz_bufio *);

int
czB_getc(cz_bufio *);

#endif
