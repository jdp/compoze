#ifndef LEXER_H
#define LEXER_H

#define MAX_SIZET ((size_t)(~(size_t)0)-2)
#define DELIM "()[]:;'"

enum
{
	T_INIT = -2,
	T_EOF,
	T_NUMBER,
	T_STRING,
	T_WORD,
	T_FREEZE,
	T_BQUOTE,
	T_EQUOTE,
	T_BDEF,
	T_EDEF,
	T_EOL
};

typedef struct cz_lexer
{
	cz_bufio *in;
	size_t    bufused;
	size_t    bufsize;
	char     *buffer;
	char      current;
} Lexer;

Lexer *
Lexer_new(cz_bufio *);

int
Lexer_destroy(Lexer *);

int
Lexer_scan(Lexer *);

#endif /* LEXER_H */

