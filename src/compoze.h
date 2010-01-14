#ifndef COMPOZE_H
#define COMPOZE_H

#include <stdio.h>
#include <stdlib.h>
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

typedef struct cz_object *(*cz_methodfn)(struct cz_state *, struct cz_object *, ...);
typedef cz_methodfn CzMethod;

typedef enum
{
	CZ_T_Nil,
	CZ_T_Boolean,
	CZ_T_VTable,
	CZ_T_Object,
	CZ_T_Symbol,
	CZ_T_Word,
	CZ_T_Number,
	CZ_T_String,
	CZ_T_List,
	CZ_T_Pair,
	CZ_T_Table,
	CZ_T_Quotation,
	CZ_T_User,
	CZ_T_MAX
} CzType;

/*
 * Basically, primitives fall under Object pointers less than 7.
 * The nil primitive is 0 because it has no bits set.
 * The true and false primitives are 6 and 2, respectively, because
 *   each value has bit #2 set (0b110 and 0b010). That makes it easy to check
 *   if a primitive value is boolean.
 */
#define CZ_NIL             ((CzObject *)0)
#define CZ_UNDEFINED       ((CzObject *)1)
#define CZ_TRUE            ((CzObject *)2)
#define CZ_FALSE           ((CzObject *)6)
#define CZ_IS_PRIMITIVE(o) ((CzObject *)(o) < (CzObject *)7)
#define CZ_IS_NIL(o)       ((CzObject *)(o) == CZ_NIL)
#define CZ_IS_BOOL(o)      ((unsigned int)(o) & 2)

#define CZ_VTABLE(T)    (cz->vtables[CZ_T_##T])

#define CZ_OBJECT_HEADER     \
	int               type;  \
	struct cz_vtable *vt;    \
	size_t            hash;

#define CZ_MAKE_OBJECT(T) ({          \
	Cz##T *o = CZ_ALLOC(Cz##T);       \
	o->type  = CZ_T_##T;              \
	o->vt    = cz->vtables[CZ_T_##T]; \
	o;                                \
})

#define CZ_AS(T,O)      ((Cz##T *)(O))
#define CZ_OBJECT(o)    ((struct cz_object *)(o))
#define CZ_SYMBOL(s)    (Symbol_intern(cz, s))
#define CZ_NUMBER(o)    ((struct cz_number *)(o))
#define CZ_QUOTATION(o) ((struct cz_quotation *)(o))

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
	char *string;
	struct cz_object *frozen;
} CzSymbol;

typedef struct cz_number
{
	CZ_OBJECT_HEADER
	union
	{
		int ival;
		double dval;
	};
} CzNumber;

typedef struct cz_list
{
	CZ_OBJECT_HEADER
	size_t size;
	size_t cap;
	struct cz_object **items;
} CzList;

typedef struct cz_pair
{
	CZ_OBJECT_HEADER
	size_t key_hash;
	struct cz_object *key;
	struct cz_object *value;
	struct cz_pair *prev;
	struct cz_pair *next;
} CzPair;

typedef struct cz_table
{
	CZ_OBJECT_HEADER
	int prime;
	size_t size;
	size_t cap;
	struct cz_object **items;
} CzTable;

typedef struct cz_quotation
{
	CZ_OBJECT_HEADER
	size_t size;
	size_t cap;
	struct cz_object **items;
} CzQuotation;

typedef struct cz_stack
{
	CZ_OBJECT_HEADER
	int top;
	int size;
	struct cz_object **items;
} CzStack;

typedef struct cz_state
{
	CZ_OBJECT_HEADER
	struct cz_vtable *vtables[CZ_T_MAX];
	struct cz_table *symbols, *strings;
	struct cz_stack *stack;
} CzState;

#define send(RCV, MSG, ARGS...) ({                   \
	struct cz_object *r = (struct cz_object *)(RCV); \
	cz_methodfn       m = bind(cz, r, (MSG));        \
	if (CZ_IS_PRIMITIVE(m)) {                        \
		printf("what the fuck %p!!!\n", m);          \
	}                                                \
	m(cz, r, ##ARGS);                                \
})

#define send2(MSG) ({ \
	struct cz_object *r = Stack_pop(cz->stack); \
	cz_methodfn m = bind(cz, r, (MSG)); \
	if (!CZ_IS_PRIMITIVE(m)) { \
		m(cz, r); \
	} \
})

#define CZ_PUSH(o) (Stack_push(cz->stack, (CzObject *)(o)))
#define CZ_POP()   (Stack_pop(cz->stack))

unsigned int djb2_hash(void *, size_t);

/* Objects */

int
bootstrap(CzState *);

inline void *
alloc(size_t);

CzObject *
VTable_lookup(CzState *, CzVTable *, CzObject *);

CzVTable *
VTable_delegated(CzState *, CzVTable *);

CzObject *
VTable_allocate(CzState *, CzVTable *);

CzMethod
VTable_add_method(CzState *, CzVTable *, CzObject *, CzMethod);

CzMethod
bind(CzState *, CzObject *, CzObject *);

CzObject *
Symbol_intern(CzState *, char *);

CzObject *
Object_hash(CzState *);

CzObject *
Object_same(CzState *);

/* Stacks */

#define Stack_peek(s)  ((s)->items[(s)->top-1])
#define Stack_empty(s) ((s)->top == 0)

CzStack *
Stack_new(unsigned int);

int
Stack_destroy(CzStack *);

int
Stack_reset(CzStack *);

int
Stack_push(CzStack *, CzObject *);

int
Stack_push_bulk(CzStack *, ...);

CzObject *
Stack_pop(CzStack *);

int
Stack_swap(CzStack *s);

/* Tables */

CzObject *
Table_create_(CzState *);

CzObject *
Table_insert_(CzState *, CzObject *, size_t, void *, void *);

CzObject *
Table_insert_pair_(CzState *, CzObject *, CzObject *);

CzObject *
Table_resize_(CzState *, CzObject *);

CzObject *
Table_lookup_(CzState *, CzObject *, size_t, CzObject *);

CzObject *
Table_insert(CzState *);

CzObject *
Table_lookup(CzState *);

CzObject *
Pair_create_(CzState *, CzObject *, CzObject *, CzObject *);

void
cz_bootstrap_table(CzState *);

/* Quotations */

CzObject *
Quotation_new(CzState *);

CzObject *
Quotation_append(CzState *, CzObject *);

CzObject *
Quotation_eval(CzState *, CzObject *);

void
cz_bootstrap_quotation(CzState *);

/* Numbers */

CzObject *
Number_create_(CzState *, int);

CzObject *
Number_hash(CzState *, CzObject *);

CzObject *
Number_equals(CzState *, CzObject *);

void
cz_bootstrap_number(CzState *);

#endif /* COMPOZE_H */

