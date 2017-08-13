#define LLIST_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

typedef struct {
    ptr *head;
    ptr *tail;
    uint64 len;
    const meta *items_desc;
} llist;

#define llist__type_id ((type_id const)32)
META_DECL(llist);

void llist_init(llist *self, const meta *type);
void llist_deinit(llist *self);

bool llist_front_pop(llist *self, const meta *type, ptr item);
void llist_front_push(llist *self, const meta *type, ptr item);
bool llist_back_pop(llist *self, const meta *type, ptr item);
void llist_back_push(llist *self, const meta *type, ptr item);
