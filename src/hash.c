#include <stdio.h>
#include "hash.h"

/*
 * DJB hash, widely used
 * http://www.cse.yorku.ca/~oz/hash.html
 */
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
 * The DEK hash function
 * From TAOCP Volume 3
 */
unsigned int
dek_hash(void *key, size_t len)
{
	unsigned char *str = key;
	unsigned int hash = len;
	size_t i = 0;
	for(i = 0; i < len; str++, i++) {
		hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
	}
	return hash;
}

/*
 * Murmur hash function
 * http://murmurhash.googlepages.com/
 */
unsigned int
murmur_hash(void *key, size_t len, unsigned int seed)
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	
	unsigned int h = seed ^ len;

	const unsigned char * data = (const unsigned char *)key;

	while (len >= 4) {
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

/*
 * The Jenkins hash function
 * http://en.wikipedia.org/wiki/Jenkins_hash_function
 */
unsigned int
jenkins_hash(void *key, size_t len)
{
	unsigned char *str = key;
    unsigned int hash = 0;
    size_t i;
 
    for (i = 0; i < len; i++) {
        hash += str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    
    return hash;
}

