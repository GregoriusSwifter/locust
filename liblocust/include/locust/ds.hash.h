#define HASH_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

typedef struct {
    uint64 p0;
    uint64 p1;
    uint64 p2;
    uint64 p3;
} hash256;

hash256 map_hash(byte *data, uint64 len);
