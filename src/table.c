#include "compoze.h"

/*
 * Credit for primes table: Aaron Krowne
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

unsigned int
djb2_hash(void *key, size_t len)
{
	unsigned char *str = key;
	unsigned int hash = 5381;
	size_t i;
	for (i = 0; i < len; i++, str++) {
		hash = ((hash << 5) + hash) + (*str);
	}
	return hash;
}

/*
 * Creates a new Pair object.
 */
CzObject *
Pair_create_(CzState *cz, CzObject *hash, CzObject *key, CzObject *value)
{
	CzPair *self   = CZ_MAKE_OBJECT(Pair);
	self->key_hash = (size_t)hash;
	self->key      = key;
	self->value    = value;
	return CZ_AS(Object, self);
}

/*
 * Creates a new Table object.
 */
CzObject *
Table_create_(CzState *cz)
{
	CzTable *self = CZ_MAKE_OBJECT(Table);
	self->prime   = 0;
	self->size    = 0;
	self->cap     = primes[0];
	self->items   = (CzObject **)CZ_CALLOC(self->cap, sizeof(CzObject *));
	return CZ_AS(Object, self);
}

/*
 * Inserts anything into the table.
 * This is super dangerous.
 * Also super useful.
 */
CzObject *
Table_insert_(CzState *cz, CzObject *self, size_t hash, void *key, void *value)
{
	CzObject *pair;
	pair = Pair_create_(cz, CZ_AS(Object, hash), CZ_AS(Object, key), CZ_AS(Object, value));
	Table_insert_pair_(cz, self, pair);
	return self;
}

/*
 * Inserts a pair by pre-calculated hash into the table.
 * Reserved mostly for internal use.
 */
CzObject *
Table_insert_pair_(CzState *cz, CzObject *self, CzObject *pair)
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
CzObject *
Table_resize_(CzState *cz, CzObject *self)
{
	CzTable *t;
	CzObject **new_items;
	size_t i;
	
	t = (CzTable *)self;
	if ((t->size / t->cap) >= 0.7) {
		t->cap = primes[++(t->prime)];
	}
	new_items = (CzObject **)CZ_CALLOC(t->cap, sizeof(CzObject *));
	if (new_items == NULL) {
		t->cap = primes[--(t->prime)];
		return CZ_NIL;
	}
	memset(new_items, 0, t->cap * sizeof(CzObject *));
	for (i = 0; i < t->size; i++) {
		new_items[((size_t)(CZ_AS(Pair, t->items[i])->key_hash) % t->cap)] = t->items[i];
	} 
	t->items = new_items;
	return self;
}

CzObject *
Table_lookup_(CzState *cz, CzObject *self, size_t hash, CzObject *key)
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
CzObject *
Table_insert(CzState *cz)
{
	CzObject *self, *key, *value, *hash, *pair;
	
	self = CZ_POP();
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
CzObject *
Table_lookup(CzState *cz)
{
	CzObject *self, *key, *hash;
	CzPair *pair;
	
	self = CZ_POP();
	key = CZ_POP();

	hash = send(key, CZ_SYMBOL("hash"));
	pair = CZ_AS(Pair, CZ_AS(Table, self)->items[(size_t)hash % CZ_AS(Table, self)->cap]);
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
	CZ_VTABLE(Pair)  = VTable_delegated(cz, CZ_VTABLE(Object));
	CZ_VTABLE(Table) = VTable_delegated(cz, CZ_VTABLE(Object));
}
	
