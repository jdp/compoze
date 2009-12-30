#ifndef HASH_H
#define HASH_H

typedef unsigned int (*hashfn_t)(void *, size_t, ...);

unsigned int
djb2_hash(void *, size_t);

unsigned int
dek_hash(void *, size_t);

unsigned int
murmur_hash(void *, size_t, unsigned int);

unsigned int
jenkins_hash(void *, size_t);

#endif
