#ifndef TABLE_H
#define TABLE_H

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

#endif
