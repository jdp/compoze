#ifndef CZ_LIST_H
#define CZ_LIST_H

Object *
List_new(CzState *);

Object *
List_at(CzState *, Object *, Object *);

Object *
List_append(CzState *, Object *, Object *);

Object *
List_pop(CzState *, Object *);

#endif /* CZ_LIST_H */
