#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compoze.h"
#include "bufio.h"
#include "object.h"
#include "lexer.h"
#include "parser.h"
#include "stack.h"
#include "number.h"
#include "quotation.h"

Object *
cz_quotation_new(CzState *cz)
{
	if (CZ_PUSH(Quotation_new(cz)) == CZ_ERR) {
		return CZ_NIL;
	}
	return CZ_PEEK();
}

Object *
cz_quotation_append(CzState *cz)
{
	Quotation *q;
	q = CZ_QUOTATION(CZ_POP());
	if (CZ_VTYPE(q) != CZ_VTABLE(CZ_TQUOTATION)) {
		CZ_ERROR(
