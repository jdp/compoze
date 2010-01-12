#ifndef QUOTATION_H
#define QUOTATION H

Object *
Quotation_new(CzState *);

Object *
Quotation_append(CzState *);

Object *
Quotation_eval(CzState *);

#endif
