#ifndef COMPOZE_H
#define COMPOZE_H

enum { CZ_ERR, CZ_OK };

struct cz_vtable;
struct cz_object;
struct cz_symbol;

typedef struct cz_object *(*cz_method)(struct cz_object *receiver, ...);
typedef cz_method Method;

#define CZ_OBJECT_HEADER      \
	struct cz_vtable *_vt[0]; \
	int               refct

typedef struct cz_vtable
{
	CZ_OBJECT_HEADER;
	int                size;
	int                tally;
	struct cz_object **keys;
	struct cz_object **values;
	struct cz_vtable  *parent;
} VTable;

typedef struct cz_object {
	CZ_OBJECT_HEADER;
} Object;

typedef struct cz_symbol
{
	CZ_OBJECT_HEADER;
	char *string;
} Symbol;

typedef struct cz_node
{
	CZ_OBJECT_HEADER;
	int             type;
	char           *value;
	struct cz_node *next;
	struct cz_node *prev;
	struct cz_node *children;
} Node;

typedef struct cz_number
{
	CZ_OBJECT_HEADER;
	union
	{
		int ival;
		double dval;
	};
} Number;

typedef struct cz_quotation
{
	CZ_OBJECT_HEADER;
	int                size;
	int                cap;
	struct cz_object **items;
} Quotation;

#define send(RCV, MSG, ARGS...) ({                   \
	struct cz_object *r = (struct cz_object *)(RCV); \
	Method            m = bind(r, (MSG));            \
	m(r, ##ARGS);                                    \
})

#define incref(o) (o->refct++)
#define decref(o) (o->refct--)

#define CZ_NIL      ((Object *)0)
#define CZ_TRUE     ((Object *)1)
#define CZ_FALSE    ((Object *)2)
#define CZ_ISNIL(o) ((Object *)o == CZ_NIL)

#endif /* COMPOZE_H */

