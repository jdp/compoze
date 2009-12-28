#ifndef COMPOZE_H
#define COMPOZE_H

enum { CZ_ERR, CZ_OK };

struct cz_vtable;
struct cz_object;
struct cz_symbol;
struct cz_state;

typedef struct cz_object *(*cz_method)(struct cz_state *, struct cz_object *, ...);
typedef cz_method Method;

typedef enum
{
	CZ_TNIL,
	CZ_TBOOLEAN,
	CZ_TVTABLE,
	CZ_TOBJECT,
	CZ_TSYMBOL,
	CZ_TNUMBER,
	CZ_TSTRING,
	CZ_TLIST,
	CZ_TPAIR,
	CZ_TTABLE,
	CZ_TQUOTATION,
	CZ_TUSER
} CzType;

/*
 * Basically, primitives fall under Object pointers less than 7.
 * The nil primitive is 0 because it has no bits set.
 * The true and false primitives are 6 and 2, respectively, because
 *   each value has bit #2 set (0b110 and 0b010). That makes it easy to check
 *   if a primitive value is boolean.
 */
#define CZ_NIL   ((Object *)0)
#define CZ_ZERO  ((Object *)1)
#define CZ_TRUE  ((Object *)2)
#define CZ_FALSE ((Object *)6)

#define CZ_IS_NIL(o)  ((Object *)o == CZ_NIL)
#define CZ_IS_BOOL(o) ((unsigned int)o & 2)

#define CZ_VTYPE(x)     (((struct Object *)(x))->vt)
#define CZ_TYPE_ID(t)   ((t)-CZ_TNIL)
#define CZ_VTABLE(t)    (CZ_FLEX_AT(cz->vts, PN_TYPE_ID(t)))

#define CZ_OBJECT_HEADER      \
	struct cz_vtable *_vt[0]; \
	int               refct;  \
	struct cz_object *hash;

typedef struct cz_vtable
{
	CZ_OBJECT_HEADER
	int                type;
	int                size;
	int                tally;
	struct cz_object **keys;
	struct cz_object **values;
	struct cz_vtable  *parent;
} VTable;

typedef struct cz_object
{
	CZ_OBJECT_HEADER
} Object;

typedef struct cz_symbol
{
	CZ_OBJECT_HEADER
	char *string;
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
	unsigned int       size;
	unsigned int       cap;
	struct cz_object **items;
} List;

typedef struct cz_pair
{
	CZ_OBJECT_HEADER
	struct cz_object *key;
	struct cz_object *value;
} Pair;

typedef struct cz_table
{
	CZ_OBJECT_HEADER
	unsigned int     size;
	unsigned int     cap;
	struct cz_pair **entries;
} Table;

typedef struct cz_quotation
{
	CZ_OBJECT_HEADER;
	unsigned int       size;
	unsigned int       cap;
	struct cz_object **items;
} Quotation;

#define incref(o) (o->refct++)
#define decref(o) (o->refct--)

#define send(RCV, MSG, ARGS...) ({                   \
	struct cz_object *r = (struct cz_object *)(RCV); \
	cz_method         m = bind(cz, r, (MSG));        \
	m(cz, r, ##ARGS);                                \
})

#define djb2_hash(s) ({ \
	unsigned char *_s = (unsigned char *)s; \
	int hash = 5381; \
	int c; \
	while ((c = *_s++)) { \
		hash = ((hash << 5) + hash) + c; \
	} \
	(struct cz_object *)hash; \
})

typedef struct cz_state
{
	CZ_OBJECT_HEADER
	VTable *vtable_vt,
	       *object_vt,
	       *symbol_vt,
	       *number_vt,
	       *string_vt,
	       *list_vt,
	       *pair_vt,
	       *table_vt,
	       *quotation_vt;
	VTable *symbols;
	List   *stack;
} CzState;

#endif /* COMPOZE_H */

