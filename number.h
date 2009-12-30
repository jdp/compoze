#ifndef NUMBER_H
#define NUMBER_H

Object *
Number_new(CzState *, int);

Object *
Number_hash(CzState *, Object *);

Object *
Number_equals(CzState *, Object *, Object *);

void
cz_bootstrap_number(CzState *);

#endif
