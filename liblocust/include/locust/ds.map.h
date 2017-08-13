#define MAP_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef HASH_H
#include <locust/ds.hash.h>
#endif

typedef struct {
    ptr items;
    const meta *items_desc;
    uint32 len;
    uint32 cap;
    uint32 items_stride;
} map;

#define map__type_id ((type_id const)31)
META_DECL(map);

void map_init(map *self, const meta *type, uint64 reserved);
void map_deinit(map *self);

bool map_set(map *self, uint32 key, const meta *type, ptr item);
bool map_get(map *self, uint32 key, const meta *type, ptr item);
bool map_pop(map *self, uint32 key, const meta *type, ptr item);
bool map_del(map *self, uint32 key);
bool map_has(map *self, uint32 key);




















