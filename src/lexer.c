#include "compoze.h"
#include "bufio.h"
#include "lexer.h"

/*
 * Creates a new lexer from a buffered I/O stream.
 */
Lexer *
Lexer_new(cz_bufio *in)
{
	Lexer *l = (Lexer *)GC_MALLOC(sizeof(Lexer));
	if (l == NULL) {
		return NULL;
	}
	l->in = in;
	l->bufsize = 8096;
	l->bufused = -1;
	l->buffer = (char *)GC_MALLOC(sizeof(char)*l->bufsize);
	if (l->buffer == NULL) {
		return NULL;
	}
	memset(l->buffer, 0, l->bufsize);
	l->current = T_INIT;
	return l;
}

/*
 * Destroys a lexer.
 */
int
Lexer_destroy(Lexer *l)
{
	if (l == NULL) {
		return CZ_ERR;
	}
	GC_FREE(l->buffer);
	GC_FREE(l);
	return CZ_OK;
}

/*
 * Gets the next character available from the lexer's input stream.
 */
#define next(l) (l->current = czB_getc(l->in))

/*
 * Resets the token buffer to an empty state for the next token
 * to be constructed.
 */
#define reset(l) (l->bufused = 0)

/*
 * Saves a piece of the token currently being built by the lexer.
 */
static int
save(Lexer *l, int c)
{
	if (l->bufused + 1 > l->bufsize) {
		size_t newsize;
		if (l->bufsize >= MAX_SIZET/2) {
			return CZ_ERR;
		}
		newsize = l->bufsize * 2;
		l->buffer = (char *)GC_REALLOC(l->buffer, sizeof(char) * newsize);
		if (l->buffer == NULL) {
			return CZ_ERR;
		}
		l->bufsize = newsize;
	}
	l->buffer[l->bufused++] = (char)c;
	return CZ_OK;
}

static void
scan_comment(Lexer *l)
{
	while (l->current != '\r' && l->current != '\n' && l->current != EOF) {
		next(l);
	}
}

static void
scan_signature(Lexer *l)
{
	int depth;
	depth = 1;
	next(l);
	while ((depth > 0) && (l->current != EOF)) {
		switch (l->current) {
			case '(':
				depth++;
				break;
			case ')':
				depth--;
				break;
		}
		next(l);
	}
}

static void
scan_number(Lexer *l)
{
	while ((isdigit(l->current) || l->current == '.' || l->current == 'e')
	       && (l->current != EOF)) {
		save(l, l->current);
		next(l);
	}
	save(l, '\0');
}

static void
scan_word(Lexer *l)
{
	while (!isspace(l->current) && (strchr(DELIM, l->current) == NULL)
		   && (l->current != EOF)) {
		save(l, l->current);
		next(l);
	}
	save(l, '\0');
}

/*
 * Places a token in the current lexer state.
 * Returns an integer T_* constant.
 */
int
Lexer_scan(Lexer *l)
{
	if (l->current == T_INIT) {
		next(l);
	}
	for (;;) {
		#ifdef DEBUG
		printf("current: '%c' %X\n", l->current, l->current);
		#endif
		switch (l->current) {
			case '\n':
			case '\r':
				next(l);
				return T_EOL;
			case ' ':
			case '\t':
				next(l);
				break;
			case '#':
				scan_comment(l);
				next(l);
				break;
			case '(':
				scan_signature(l);
				next(l);
				break;
			case ':':
				next(l);
				return T_BDEF;
			case ';':
				next(l);
				return T_EDEF;
			case '[':
				next(l);
				return T_BQUOTE;
			case ']':
				next(l);
				return T_EQUOTE;
			case '\0':
			case EOF:
				return T_EOF;
			default:
				reset(l);
				if (isdigit(l->current)) {
					scan_number(l);
					return T_NUMBER;
				}
				else {
					scan_word(l);
					return T_WORD;
				}
		}
	}
}
	
