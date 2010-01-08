#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compoze.h"
#include "object.h"
#include "table.h"

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

Object *
Pair_new(CzState *cz, Object *hash, Object *key, Object *value)
{
	Pair *self     = (Pair *)VTable_allocate(cz, CZ_VTABLE(CZ_TPAIR), sizeof(Pair));
	self->vt  = CZ_VTABLE(CZ_TPAIR);
	self->key_hash = (size_t)hash;
	self->key      = key;
	self->value    = value;
	return (Object *)self;
}

Object *
Table_new(CzState *cz)
{
	Table *self   = (Table *)VTable_allocate(cz, CZ_VTABLE(CZ_TTABLE), sizeof(Table));
	self->vt = CZ_VTABLE(CZ_TTABLE);
	self->prime   = 0;
	self->size    = 0;
	self->cap     = primes[0];
	self->items   = (Object **)calloc(self->cap, sizeof(Object *));
	return (Object *)self;
}

/*
 * Inserts anything into the table.
 * This is super dangerous.
 * Also super useful.
 */
Object *
Table_insert_raw(CzState *cz, Object *self, size_t hash, void *key, void *value)
{
	Object *pair;
	pair = Pair_new(cz, (Object *)hash, (Object *)key, (Object *)value);
	Table_insert_pair(cz, self, pair);
	return self;
}

/*
 * Inserts a pair by pre-calculated hash into the table.
 * Reserved mostly for internal use.
 */
Object *
Table_insert_pair(CzState *cz, Object *self, Object *pair)
{
	Table *t;
	size_t i;
	
	t = (Table *)self;
	if ((++(t->size) / t->cap) > 0.7) {
		Table_resize(cz, self);
	}
	i = (size_t)(((Pair *)pair)->key_hash) % t->cap;
	((Pair *)pair)->next = (Pair *)t->items[i];
	t->items[i] = pair;
	return self;
}

/*
 * Associates an object key with a value in the table.
 */
Object *
Table_insert(CzState *cz, Object *self, Object *key, Object *value)
{
	Object *hash, *pair;
	
	hash = send(key, CZ_SYMBOL("hash"));
	pair = Pair_new(cz, hash, key, value);
	Table_insert_pair(cz, self, pair);
	return self;
}

/*
 * Grows a table as needed.
 */
Object *
Table_resize(CzState *cz, Object *self)
{
	Table *t;
	Object **new_items;
	size_t i;
	
	t = (Table *)self;
	if ((t->size / t->cap) >= 0.7) {
		t->cap = primes[++(t->prime)];
	}
	new_items = (Object **)calloc(t->cap, sizeof(Object *));
	if (new_items == NULL) {
		t->cap = primes[--(t->prime)];
		return CZ_NIL;
	}
	memset(new_items, 0, t->cap * sizeof(Object *));
	for (i = 0; i < t->size; i++) {
		new_items[((size_t)(((Pair *)t->items[i])->key_hash) % t->cap)] = t->items[i];
	} 
	free(t->items);
	t->items = new_items;
	return self;
}

Object *
Table_lookup_raw(CzState *cz, Object *self, size_t hash, Object *key)
{
	Pair *pair;
	
	pair = (Pair *)((Table *)self)->items[hash % ((Table *)self)->cap];
	while (!CZ_IS_NIL(pair)) {
		if (key == pair->key) {
			return pair->value;
		}
		pair = pair->next;
	}
	
	return CZ_NIL;
}

/*
 * Returns the value associated with the key from the table.
 */
Object *
Table_lookup(CzState *cz, Object *self, Object *key)
{
	Table *t;
	Object *hash;
	Pair *pair;

	t = (Table *)self;
	hash = send(key, CZ_SYMBOL("hash"));
	pair = (Pair *)t->items[(size_t)hash % t->cap];
	while (!CZ_IS_NIL(pair)) {
		if (send(pair->key, CZ_SYMBOL("equals"), key) == CZ_TRUE) {
			return pair->value;
		}
		pair = (Pair *)pair->next;
	}
	return CZ_NIL;
}

/*
 * Bootstraps the Table and Pair kinds
 */
void
cz_bootstrap_table(CzState *cz)
{
	CZ_VTABLE(CZ_TPAIR)  = VTable_delegated(cz, CZ_VTABLE(CZ_TOBJECT));
	send(CZ_VTABLE(CZ_TPAIR), CZ_SYMBOL("addMethod"), CZ_SYMBOL("new"), Pair_new);
	CZ_VTABLE(CZ_TTABLE) = VTable_delegated(cz, CZ_VTABLE(CZ_TOBJECT));
	send(CZ_VTABLE(CZ_TTABLE), CZ_SYMBOL("addMethod"), CZ_SYMBOL("insert"), Table_insert);
	send(CZ_VTABLE(CZ_TTABLE), CZ_SYMBOL("addMethod"), CZ_SYMBOL("lookup"), Table_lookup);
}
	
