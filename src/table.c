#include "compoze.h"

/*
 * Credit for primes table: Aaron KOBJrowne
 * http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
 */
static const unsigned int primes[] = {
	53, 97, 193, 389,
	769, 1543, 3079, 6151,
	12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869,
	3145739, 6291469, 12582917, 25165843,
	50331653, 100663319, 201326611, 402653189,
	805306457, 1610612741
};

OBJ
djb2_hash(void *key, size_t len)
{
	unsigned char *str = key;
	OBJ hash = 5381;
	size_t i;
	for (i = 0; i < len; i++, str++) {
		hash = ((hash << 5) + hash) + (*str);
	}
	return hash;
}

/*
 * Creates a new Pair object.
 */
OBJ
Pair_create_(CzState *cz, OBJ hash, OBJ key, OBJ value)
{
	CzPair *self   = CZ_MAKE_OBJECT(Pair);
	self->key_hash = hash;
	self->key      = key;
	self->value    = value;
	return (OBJ)self;
}

/*
 * Creates a new Table object.
 */
OBJ
Table_create_(CzState *cz)
{
	CzTable *self = CZ_MAKE_OBJECT(Table);
	self->prime   = 0;
	self->size    = 0;
	self->cap     = primes[0];
	self->items   = (OBJ*)CZ_CALLOC(self->cap, sizeof(OBJ));
	return (OBJ)self;
}

/*
 * Inserts anything into the table.
 * This is super dangerous.
 * Also super useful.
 */
OBJ
Table_insert_(CzState *cz, OBJ self, OBJ hash, OBJ key, OBJ value)
{
	OBJ pair;
	//printf("attempting to insert %s\n", CZ_AS(Symbol, key)->string);
	pair = Pair_create_(cz, (OBJ)hash, (OBJ)key, (OBJ)value);
	Table_insert_pair_(cz, self, pair);
	//printf("inserted %s\n", CZ_AS(Symbol, key)->string);
	return self;
}

/*
 * Inserts a pair by pre-calculated hash into the table.
 * Reserved mostly for internal use.
 */
OBJ
Table_insert_pair_(CzState *cz, OBJ self, OBJ pair)
{
	CzTable *t;
	size_t i;
	
	t = CZ_AS(Table, self);
	if ((++(t->size) / t->cap) > 0.7) {
		Table_resize_(cz, self);
	}
	i = (size_t)(CZ_AS(Pair, pair)->key_hash) % t->cap;
	CZ_AS(Pair, pair)->next = CZ_AS(Pair, t->items[i]);
	t->items[i] = pair;
	return self;
}

/*
 * Grows a table as needed.
 */
OBJ
Table_resize_(CzState *cz, OBJ self)
{
	CzTable *t;
	OBJ *new_items;
	size_t i;
	
	t = (CzTable *)self;
	if ((t->size / t->cap) >= 0.7) {
		t->cap = primes[++(t->prime)];
	}
	new_items = (OBJ*)CZ_CALLOC(t->cap, sizeof(OBJ));
	if (new_items == NULL) {
		t->cap = primes[--(t->prime)];
		return CZ_NIL;
	}
	memset(new_items, 0, t->cap * sizeof(OBJ));
	for (i = 0; i < t->size; i++) {
		new_items[((size_t)(CZ_AS(Pair, t->items[i])->key_hash) % t->cap)] = t->items[i];
	} 
	t->items = new_items;
	return self;
}

OBJ
Table_lookup_(CzState *cz, OBJ self, OBJ hash, OBJ key)
{
	CzPair *pair;
	
	pair = CZ_AS(Pair, CZ_AS(Table, self)->items[hash % CZ_AS(Table, self)->cap]);
	while (!CZ_IS_NIL(pair)) {
		if (key == pair->key) {
			return pair->value;
		}
		pair = pair->next;
	}

	return CZ_UNDEFINED;
}

/*
 * Associates an object key with a value in the table.
 * ( V K T -- T )
 */
OBJ
Table_insert(CzState *cz, OBJ self)
{
	OBJ key, value, hash, pair;
	
	key = CZ_POP();
	value = CZ_POP();
	
	hash = send(key, CZ_SYMBOL("hash"));
	pair = Pair_create_(cz, hash, hash, value);
	Table_insert_pair_(cz, self, pair);
	
	CZ_PUSH(self);
	
	return self;
}

/*
 * Returns the value associated with the key from the table.
 */
OBJ
Table_lookup(CzState *cz, OBJ self)
{
	OBJ key, hash;
	CzPair *pair;
	
	key = CZ_POP();

	hash = send(key, CZ_SYMBOL("hash"));
	pair = CZ_AS(Pair, CZ_AS(Table, self)->items[hash % CZ_AS(Table, self)->cap]);
	while (!CZ_IS_NIL(pair)) {
		if (send(pair->key, CZ_SYMBOL("equals"), key) == CZ_TRUE) {
			CZ_PUSH(pair->value);
			return pair->value;
		}
		pair = CZ_AS(Pair, pair->next);
	}
	CZ_PUSH(CZ_NIL);
	return CZ_UNDEFINED;
}

/*
 * Bootstraps the Table and Pair kinds
 */
void
cz_bootstrap_table(CzState *cz)
{
	CZ_VTABLE(Pair)  = VTable_delegated_(cz, CZ_VTABLE(Object));
	CZ_VTABLE(Table) = VTable_delegated_(cz, CZ_VTABLE(Object));
}
	
