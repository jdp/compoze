#ifndef COMPOZE_H
#define COMPOZE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gc/gc.h>

enum { CZ_ERR, CZ_OK };

struct cz_vtable;
struct cz_object;
struct cz_symbol;
struct cz_table;
struct cz_state;

typedef struct cz_object *(*cz_methodfn)(struct cz_state *, struct cz_object *, ...);
typedef cz_methodfn Method;

typedef enum
{
	CZ_T_NIL,
	CZ_T_BOOLEAN,
	CZ_T_VTABLE,
	CZ_T_OBJECT,
	CZ_T_SYMBOL,
	CZ_T_WORD,
	CZ_T_NUMBER,
	CZ_T_STRING,
	CZ_T_LIST,
	CZ_T_PAIR,
	CZ_T_TABLE,
	CZ_T_QUOTATION,
	CZ_T_USER,
	CZ_T_MAX
} CzType;

/*
 * Basically, primitives fall under Object pointers less than 7.
 * The nil primitive is 0 because it has no bits set.
 * The true and false primitives are 6 and 2, respectively, because
 *   each value has bit #2 set (0b110 and 0b010). That makes it easy to check
 *   if a primitive value is boolean.
 */
#define CZ_NIL             ((Object *)0)
#define CZ_ZERO            ((Object *)1)
#define CZ_TRUE            ((Object *)2)
#define CZ_FALSE           ((Object *)6)
#define CZ_IS_PRIMITIVE(o) ((Object *)(o) < (Object *)7)
#define CZ_IS_NIL(o)       ((Object *)(o) == CZ_NIL)
#define CZ_IS_BOOL(o)      ((unsigned int)(o) & 2)
#define CZ_IS_NUMBER(o)    ((o)->vt == cz->vtables[CZ_T_NUMBER])

#define CZ_VTYPE(x)     (((struct Object *)(x))->vt)
#define CZ_VTYPE_ID(t)  ((unsigned int)((t)-CZ_T_NIL))
#define CZ_VTABLE(t)    (cz->vtables[CZ_VTYPE_ID(t)])

#define CZ_OBJECT_HEADER     \
	int               type;  \
	struct cz_vtable *vt;    \
	int               refct; \
	size_t            hash;

#define CZ_OBJECT(o)    ((struct cz_object *)(o))
#define CZ_SYMBOL(s)    (Symbol_intern(cz, s))
#define CZ_NUMBER(o)    ((struct cz_number *)(o))
#define CZ_QUOTATION(o) ((struct cz_quotation *)(o))

typedef struct cz_vtable
{
	CZ_OBJECT_HEADER
	struct cz_table   *table;
	struct cz_vtable  *parent;
} VTable;

typedef struct cz_object
{
	CZ_OBJECT_HEADER
} Object;

typedef struct cz_symbol
{
	CZ_OBJECT_HEADER
	char             *string;
	struct cz_object *frozen;
} Symbol;

typedef struct cz_number
{
	CZ_OBJECT_HEADER
	union
	{
		int ival;
		double dval;
	};
} Number;

typedef struct cz_list
{
	CZ_OBJECT_HEADER
	size_t       size;
	size_t       cap;
	struct cz_object **items;
} List;

typedef struct cz_pair
{
	CZ_OBJECT_HEADER
	size_t            key_hash;
	struct cz_object *key;
	struct cz_object *value;
	struct cz_pair   *prev;
	struct cz_pair   *next;
} Pair;

typedef struct cz_table
{
	CZ_OBJECT_HEADER
	int        prime;
	size_t     size;
	size_t     cap;
	struct cz_object **items;
} Table;

typedef struct cz_quotation
{
	CZ_OBJECT_HEADER;
	size_t       size;
	size_t       cap;
	struct cz_object **items;
} Quotation;

typedef struct cz_stack
{
	int      top;
	int      size;
	Object **items;
} Stack;

typedef struct cz_state
{
	CZ_OBJECT_HEADER
	VTable *vtables[CZ_T_MAX];
	Table  *symbols, *strings;
	Stack  *stack;
} CzState;

#define incref(o) (o->refct++)
#define decref(o) (o->refct--)

#define send(RCV, MSG, ARGS...) ({                   \
	struct cz_object *r = (struct cz_object *)(RCV); \
	cz_methodfn       m = bind(cz, r, (MSG));        \
	m(cz, r, ##ARGS);                                \
})

#define apisend(RCV, MSG, ARGS...) ({                \
	struct cz_object *r = (struct cz_object *)(RCV); \
	cz_methodfn       m = bind(cz, r, (MSG));        \
	Stack_push_bulk(cz->stack, ##ARGS);              \
	m(cz, r);                                        \
})

#define CZ_PUSH(o) (Stack_push(cz->stack, (Object *)(o)))
#define CZ_POP()   (Stack_pop(cz->stack))

unsigned int djb2_hash(void *, size_t);

/* Objects */

int
bootstrap(CzState *);

inline void *
alloc(size_t);

Object *
VTable_lookup(CzState *, VTable *, Object *);

VTable *
VTable_delegated(CzState *, VTable *);

Object *
VTable_allocate(CzState *, VTable *, int);

Method
VTable_add_method(CzState *, VTable *, Object *, Method);

Method
bind(CzState *, Object *, Object *);

Object *
Symbol_intern(CzState *, char *);

Object *
Symbol_hash(CzState *, Object *);

Object *
Symbol_equals(CzState *, Object *, Object *);

/* Stacks */

#define Stack_peek(s)  ((s)->items[(s)->top-1])
#define Stack_empty(s) ((s)->top == 0)

Stack *
Stack_new(unsigned int);

int
Stack_destroy(Stack *);

int
Stack_reset(Stack *);

int
Stack_push(Stack *, Object *);

int
Stack_push_bulk(Stack *, ...);

Object *
Stack_pop(Stack *);

int
Stack_swap(Stack *s);

/* Tables */

Object *
Table_new(CzState *);

Object *
Table_insert(CzState *, Object *, Object *, Object *);

Object *
Table_lookup(CzState *, Object *, Object *);

Object *
Pair_create_(CzState *, Object *, Object *, Object *);

Object *
Table_insert_(CzState *, Object *, size_t, void *, void *);

Object *
Table_insert_pair_(CzState *, Object *, Object *);

Object *
Table_resize_(CzState *, Object *);

Object *
Table_lookup_(CzState *, Object *, size_t, Object *);

void
cz_bootstrap_table(CzState *);

/* Quotations */

Object *
Quotation_new(CzState *);

Object *
Quotation_append(CzState *);

Object *
Quotation_eval(CzState *);

/* Numbers */

Object *
Number_create_(CzState *, int);

Object *
Number_hash(CzState *, Object *);

Object *
Number_equals(CzState *, Object *);

void
cz_bootstrap_number(CzState *);

#endif /* COMPOZE_H */

