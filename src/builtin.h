#ifndef BUILTIN_H
#define BUILTIN_H

#define BUILTIN(N) void *czW_##N(cz_interpreter *i)

BUILTIN(true);
BUILTIN(false);
BUILTIN(eq);
BUILTIN(call);
BUILTIN(print);
BUILTIN(println);
BUILTIN(dup);
BUILTIN(swap);
BUILTIN(dip);

#endif
