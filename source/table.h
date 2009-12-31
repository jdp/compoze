#ifndef TABLE_H
#define TABLE_H

Object *
Pair_new(CzState *, Object *, Object *, Object *);

Object *
Table_new(CzState *);

Object *
Table_insert_raw(CzState *, Object *, size_t, void *, void *);

Object *
Table_insert_pair(CzState *, Object *, Object *);

Object *
Table_insert(CzState *, Object *, Object *, Object *);

Object *
Table_resize(CzState *, Object *);

Object *
Table_lookup_raw(CzState *, Object *, size_t, Object *);

Object *
Table_lookup(CzState *, Object *, Object *);

void
cz_bootstrap_table(CzState *);

#endif
