#define BAG_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

typedef struct {
    uint64 len;
    uint64 cap;
    ptr items;
    meta const *items_desc;
} bag;

#define bag__type_id ((type_id const)36)
META_DECL(bag);

void bag_init(bag *self, meta const *type);
void bag_deinit(bag *self);

bool bag_insert(bag *self, meta const *type, ptr item);
bool bag_get(bag *self, uint64 index, meta const *type, ptr item);
bool bag_remove(bag *self, uint64 index);
