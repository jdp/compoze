#include "compoze.h"

OBJ
Symbol_intern_(CzState *cz, char *string)
{
	CzSymbol *symbol;
	CzPair *pair;
	OBJ hash;
	
	hash = djb2_hash(string, strlen(string));
	pair = CZ_AS(Pair, cz->symbols->items[hash % cz->symbols->cap]);
	while (!CZ_IS_NIL(pair)) {
		if (strcmp((char *)pair->key, string) == 0) {
			return pair->value;
		}
		pair = pair->next;
	}
	symbol = CZ_MAKE_OBJECT(Symbol);
	symbol->hash = hash;
	symbol->frozen = CZ_FALSE;
	symbol->string = strdup(string);
	symbol->len = strlen(string);
	Table_insert_(cz, (OBJ)cz->symbols, hash, (OBJ)string, (OBJ)symbol);
	return (OBJ)symbol;
}

OBJ
String_create_(CzState *cz, const char *str, size_t len)
{
	CzString *string = CZ_MAKE_OBJECT(String);
	string->string = strdup(str);
	string->len = len;
	string->hash = (OBJ)djb2_hash((void *)str, strlen(str));
	string->frozen = CZ_FALSE;
	return (OBJ)string;
}

OBJ
String_create2_(CzState *cz, const char *str)
{
	return String_create_(cz, str, strlen(str));
}

OBJ
cz_sprintf(CzState *cz, const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	int len = vsnprintf(NULL, 0, fmt, arg);
	char *ptr = CZ_ALLOC_N(char, len);
	va_end(arg);
	va_start(arg, fmt);
	vsprintf(ptr, fmt, arg);
	va_end(arg);
	OBJ str = String_create_(cz, ptr, len);
	CZ_FREE(ptr);
	return str;
}

void
cz_bootstrap_string(CzState *cz)
{
	CZ_VTABLE(String) = VTable_delegated_(cz, CZ_VTABLE(Object));
}
