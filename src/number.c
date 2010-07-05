#include "compoze.h"

#define MATH(A,OP,B) CZ_INT2FIX(CZ_FIX2INT(A) OP CZ_FIX2INT(B))
#define CMP(A,OP,B)  CZ_BOOL(CZ_FIX2INT(A) OP CZ_FIX2INT(B))

OBJ
Fixnum_add(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	CZ_PUSH(MATH(other, +, self));
	return CZ_NIL;
}

OBJ
Fixnum_subtract(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	CZ_PUSH(MATH(other, -, self));
	return CZ_NIL;
}

OBJ
Fixnum_multiply(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	CZ_PUSH(MATH(other, *, self));
	return CZ_NIL;
}

OBJ
Fixnum_divide(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	CZ_PUSH(MATH(other, /, self));
	return CZ_NIL;
}

OBJ
Fixnum_less_than(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	CZ_PUSH(CMP(other, <, self));
	return CZ_NIL;
}

OBJ
Fixnum_greater_than(CzState *cz, OBJ self)
{
	OBJ other;
	other = CZ_POP();
	CZ_PUSH(CMP(other, >, self));
	return CZ_NIL;
}

OBJ
Fixnum_to_string(CzState *cz, OBJ self)
{
	OBJ str;
	str = cz_sprintf(cz, "%d", CZ_FIX2INT(self));
	CZ_PUSH(str);
	return str;
}

/*
 * Bootstraps the Fixnum type
 */
void
cz_bootstrap_fixnum(CzState *cz)
{
	CZ_VTABLE(Fixnum) = VTable_delegated_(cz, CZ_VTABLE(Object));
	cz_define_method(Fixnum, "add", Fixnum_add);
	cz_define_method(Fixnum, "+", Fixnum_add);
	cz_define_method(Fixnum, "sub", Fixnum_subtract);
	cz_define_method(Fixnum, "-", Fixnum_subtract);
	cz_define_method(Fixnum, "mul", Fixnum_multiply);
	cz_define_method(Fixnum, "*", Fixnum_multiply);
	cz_define_method(Fixnum, "div", Fixnum_divide);
	cz_define_method(Fixnum, "/", Fixnum_divide);
	cz_define_method(Fixnum, "lt", Fixnum_less_than);
	cz_define_method(Fixnum, "<", Fixnum_less_than);
	cz_define_method(Fixnum, "gt", Fixnum_greater_than);
	cz_define_method(Fixnum, ">", Fixnum_greater_than);
	cz_define_method(Fixnum, "to-string", Fixnum_to_string);
}

