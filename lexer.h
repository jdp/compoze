#ifndef LEXER_H
#define LEXER_H

#define MAX_SIZET ((size_t)(~(size_t)0)-2)
#define DELIM "()[]:;"

enum
{
	T_NUMBER,
	T_STRING,
	T_WORD,
	T_BQUOTE,
	T_EQUOTE,
	T_BDEF,
	T_EDEF,
	T_EOL,
	T_EOF
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

