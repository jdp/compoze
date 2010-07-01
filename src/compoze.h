#ifndef COMPOZE_H
#define COMPOZE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <gc/gc.h>

enum { CZ_ERR, CZ_OK };

#define NOTDEADYET           printf("-- STILL OK IN %s AT %d --\n", __FILE__, __LINE__);

#define UNUSED(E)            do { (void)(E); } while (0)

#define CZ_ALLOC(T)          (T *)CZ_MALLOC(sizeof(T))
#define CZ_ALLOC_N(T,N)      (T *)CZ_MALLOC(sizeof(T)*(N))
#define CZ_MALLOC            GC_MALLOC
#define CZ_CALLOC(m,n)       GC_MALLOC((m)*(n))
#define CZ_REALLOC           GC_REALLOC
#define CZ_FREE(s)           UNUSED(s)

struct cz_vtable;
struct cz_object;
struct cz_symbol;
struct cz_table;
struct cz_state;

typedef unsigned long OBJ;

typedef OBJ (*cz_methodfn)(struct cz_state *, OBJ, ...);
typedef cz_methodfn CzMethod;

typedef enum
{
	CZ_T_Nil,
	CZ_T_True,
	CZ_T_False,
	CZ_T_VTable,
	CZ_T_Object,
	CZ_T_Symbol,
	CZ_T_Word,
	CZ_T_Fixnum,
	CZ_T_String,
	CZ_T_Pair,
	CZ_T_Table,
	CZ_T_Quotation,
	CZ_T_Continuation,
	CZ_T_Error,
	CZ_T_User,
	CZ_T_MAX
} CzType;

#define CZ_IS_FIXNUM(F)    ((F) & 1)
#define CZ_INT2FIX(I)      (((I) << 1) | 1)
#define CZ_FIX2INT(F)      ((int)((F) >> 1))
#define CZ_NIL             ((OBJ)0)
#define CZ_UNDEFINED       ((OBJ)4)
#define CZ_TRUE            ((OBJ)2)
#define CZ_FALSE           ((OBJ)6)
#define CZ_UNWIND          ((OBJ)8)
#define CZ_IS_NIL(o)       ((OBJ)(o) == CZ_NIL)
#define CZ_IS_BOOL(o)      (((OBJ)(o) & 3) == 2)
#define CZ_IS_IMMEDIATE(o) (o == CZ_NIL || o == CZ_UNDEFINED || CZ_IS_BOOL(o) || CZ_IS_FIXNUM(o))
#define CZ_BOOL(o)         ((o) ? CZ_TRUE : CZ_FALSE)

#define CZ_VTABLE(T)    (cz->vtables[CZ_T_##T])
#define CZ_PROTO(O)     (CZ_IS_IMMEDIATE(O) ? cz->vtables[cz_proto_id(O)] : CZ_AS(Object, O)->vt)
#define CZ_IS(T,O)      (cz_proto_id(O) == CZ_T_##T)

#define CZ_OBJECT_HEADER     \
	int               type;  \
	struct cz_vtable *vt;    \
	OBJ               hash;

#define CZ_MAKE_OBJECT(T) ({          \
	Cz##T *o = CZ_ALLOC(Cz##T);       \
	o->type  = CZ_T_##T;              \
	o->vt    = cz->vtables[CZ_T_##T]; \
	o;                                \
})

#define CZ_AS(T,O)       ((Cz##T *)(O))
#define CZ_SYMBOL(s)     (Symbol_intern_(cz, s))
#define CZ_QUOTE(o, ...) (Quotation_create2_(cz, o, ##__VA_ARGS__ , CZ_UNWIND));

typedef struct cz_vtable
{
	CZ_OBJECT_HEADER
	struct cz_table *table;
	struct cz_vtable *parent;
} CzVTable;

typedef struct cz_object
{
	CZ_OBJECT_HEADER
} CzObject;

typedef struct cz_symbol
{
	CZ_OBJECT_HEADER
	char   *string;
	size_t  len;
	OBJ     frozen;
} CzSymbol;
typedef CzSymbol CzString;

typedef struct cz_pair
{
	CZ_OBJECT_HEADER
	OBJ key_hash;
	OBJ key;
	OBJ value;
	struct cz_pair *prev;
	struct cz_pair *next;
} CzPair;

typedef struct cz_table
{
	CZ_OBJECT_HEADER
	int prime;
	size_t size;
	size_t cap;
	OBJ *items;
} CzTable;

typedef struct cz_quotation
{
	CZ_OBJECT_HEADER
	size_t size;
	size_t cap;
	OBJ *items;
} CzQuotation;

typedef struct cz_error
{
	CZ_OBJECT_HEADER
	OBJ reason;
} CzError;

typedef struct cz_continuation
{
	CZ_OBJECT_HEADER
	struct cz_quotation *data_stack;
	struct cz_quotation *retain_stack;
	struct cz_quotation *call_stackl;
} CzContinuation;

typedef struct cz_state
{
	CZ_OBJECT_HEADER
	struct cz_vtable *vtables[CZ_T_MAX];
	struct cz_table *symbols, *strings;
	struct cz_quotation *data_stack, *retain_stack, *call_stack;
	int ip;
} CzState;

#define cz_define_method(T, S, M) VTable_add_method_(cz, CZ_VTABLE(T), CZ_SYMBOL(S), (CzMethod)M)

#define send(RCV, MSG, ARGS...) ({ \
	OBJ r = (OBJ)(RCV); \
	OBJ m = bind(cz, r, (MSG)); \
	if (CZ_IS_IMMEDIATE(m)) { \
		printf("what the fuck %lu!!!\n", m); \
	} \
	((CzMethod)m)(cz, r, ##ARGS); \
	CZ_POP(); \
})

#define send2(MSG) ({ \
	OBJ r = Quotation_pop_(cz, cz->data_stack); \
	OBJ m = bind(cz, r, (MSG)); \
	if (m != CZ_UNDEFINED) { \
		((CzMethod)m)(cz, r); \
	} \
	else { \
		Quotation_push_(cz, cz->data_stack, r); \
		printf("object does not respond to message\n"); \
	} \
})

#define CZ_PUSH(o)   (Quotation_push_(cz, cz->data_stack, (OBJ)(o)))
#define CZ_POP()     (Quotation_pop_(cz, cz->data_stack))
#define CZ_SWAP()    (Quotation_swap_(cz, cz->data_stack))
#define CZ_PEEK(s)   (CZ_AS(Quotation, s)->items[CZ_AS(Quotation, s)->size-1])
#define CZ_RETAIN(o) (Quotation_push_(cz, cz->retain_stack, (o)))
#define CZ_RESTORE() (Quotation_push_(cz, cz->data_stack, Quotation_pop_(cz, cz->retain_stack)))

OBJ djb2_hash(void *, size_t);

/* Helper functions */

void cz_tree(CzState *, CzQuotation *, int);

CzType cz_proto_id(OBJ);

/* Objects */

int bootstrap(CzState *);

inline void *alloc(size_t);

CzVTable *VTable_delegated_(CzState *, CzVTable *);
CzObject *VTable_allocate_(CzState *, CzVTable *);
CzMethod VTable_add_method_(CzState *, CzVTable *, OBJ, CzMethod);
OBJ VTable_lookup_(CzState *, CzVTable *, OBJ);

OBJ bind(CzState *, OBJ, OBJ);

OBJ Object_quote_(CzState *cz, OBJ obj);

OBJ Object_true(CzState *, OBJ);
OBJ Object_false(CzState *, OBJ);
OBJ Object_nil(CzState *, OBJ);
OBJ Object_hash(CzState *, OBJ);
OBJ Object_same(CzState *, OBJ);
OBJ Object_drop(CzState *, OBJ);
OBJ Object_dup(CzState *, OBJ);
OBJ Object_swap(CzState *, OBJ);
OBJ Object_quote(CzState *, OBJ);
OBJ Object_swapd(CzState *, OBJ);
OBJ Object_dupd(CzState *, OBJ);
OBJ Object_nip(CzState *, OBJ);
OBJ Object_pick(CzState *, OBJ);
OBJ Object_retain(CzState *, OBJ);
OBJ Object_restore(CzState *, OBJ);

/* Strings */

OBJ Symbol_intern_(CzState *, char *);
OBJ String_create_(CzState *, const char *, size_t);
OBJ String_create2_(CzState *, const char *);
OBJ cz_sprintf(CzState *, const char *, ...);
void cz_bootstrap_string(CzState *);

/* Tables */

OBJ Table_create_(CzState *);
OBJ Table_insert_(CzState *, CzTable *, OBJ, OBJ, OBJ);
OBJ Table_insert_pair_(CzState *, CzTable *, CzPair *);
OBJ Table_resize_(CzState *, CzTable *);
OBJ Table_lookup_(CzState *, CzTable *, OBJ, OBJ);

OBJ Table_insert(CzState *, OBJ);
OBJ Table_lookup(CzState *, OBJ);

OBJ Pair_create_(CzState *, OBJ, OBJ, OBJ);

void cz_bootstrap_table(CzState *);

/* Quotations */

OBJ  Quotation_create_(CzState *);
OBJ  Quotation_create2_(CzState *cz, OBJ, ...);
void Quotation_eval_(CzState *);
OBJ  Quotation_push_(CzState *, CzQuotation *, OBJ);
OBJ  Quotation_pop_(CzState *, CzQuotation *);
OBJ  Quotation_drop_(CzState *, CzQuotation *);
OBJ  Quotation_dup_(CzState *, CzQuotation *);
OBJ  Quotation_swap_(CzState *, CzQuotation *);
OBJ  Quotation_concat_(CzState *, CzQuotation *, CzQuotation *);
OBJ  Quotation_cons_(CzState *, CzQuotation *, OBJ);

OBJ Quotation_at(CzState *, OBJ);
OBJ Quotation_push(CzState *, OBJ);
OBJ Quotation_cons(CzState *, OBJ);
OBJ Quotation_call(CzState *, OBJ);
OBJ Quotation_dip(CzState *, OBJ);
OBJ Quotation_if(CzState *, OBJ);

void cz_bootstrap_quotation(CzState *);

/* Fixnums */

OBJ Fixnum_add(CzState *, OBJ);
OBJ Fixnum_subtract(CzState *, OBJ);
void cz_bootstrap_fixnum(CzState *);

#endif /* COMPOZE_H */

