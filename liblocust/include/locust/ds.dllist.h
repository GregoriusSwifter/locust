#define DLLIST_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

typedef struct {
    uint64 len;
    ptr head;
    ptr tail;
    meta const * items_desc;
} dllist;

#define dllist__type_id ((type_id const)33)
META_DECL(dllist);

void dllist_init(dllist *self, meta const *type);
void dllist_deinit(dllist *self);

uint64 dllist_len(dllist *self);
bool dllist_front_pop(dllist *self, meta const *type, ptr item);
void dllist_front_push(dllist *self, meta const *type, ptr item);
bool dllist_back_pop(dllist *self, meta const *type, ptr item);
void dllist_back_push(dllist *self, meta const *type, ptr item);

