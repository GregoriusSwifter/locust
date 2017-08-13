#ifndef BAG_H
#include <locust/ds.bag.h>
#endif

META_FIELDS_DEF(bag) = {
    4, {
        META_FIELD_DEF(bag, len, uint64),
        META_FIELD_DEF(bag, cap, uint64),
        META_FIELD_DEF(bag, items, ptr),
        META_FIELD_DEF(bag, items_desc, ptr),
    }
};

META_DEF(bag);

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef ATOMIC_H
#include <locust/sched.atomic.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#define BLOCK_SIZE 8
#define BLOCK_INITIAL 8

nonnullfunc void
bag_init(bag *self, const meta* type) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
#endif
    self->items_desc = type;
    self->items = mem_get(BLOCK_INITIAL, type);
    assert(self->items != nil);
    self->len = 0;
    self->cap = BLOCK_INITIAL;
    return;
}

void bag_deinit(bag *self) {
    assert(self != nil);
    mem_free(self->items);
}

nonnullfunc bool
bag_insert(bag *self, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);

    // Check for needing to enlarge
    if (self->len == self->cap) {
        ptr t = mem_resize(self->items, self->cap+BLOCK_SIZE, self->items_desc);
        if (t == NULL) {
            fprintf(stderr, "bag_insert() error: resizing failed\n");
            return false;
        }
        self->items = t;
        self->cap += BLOCK_SIZE;
    }

    // Add item
    uint64 index = atomic_fetch_and_add(&self->len, 1ULL);
    uint64 size = self->items_desc->size;
    mem_copy(item, self->items+(index*size), size);
    return true;
}

nonnullfunc bool
bag_get(bag *self, uint64 index, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);

    // Bounds check
    if (index >= self->len) {
        return false;
    }
    uint64 size = self->items_desc->size;
    mem_copy(self->items+(index*size), item, size);
    return true;
}

nonnullfunc bool
bag_remove(bag *self, uint64 index) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    // Bounds check
    if (index >= self->len) {
        return false;
    }

    ptr *items = self->items;
    ptr tmp = items[self->len-1];
    items[self->len-1] = items[index];
    items[index] = tmp;
    items[self->len-1] = NULL;
    --self->len;

    // Check for possible shrinking.
    if (((self->cap - self->len) > (BLOCK_SIZE * 2))
        && (self->cap > (BLOCK_INITIAL+BLOCK_SIZE))) {
        ptr t = mem_resize(self->items, self->cap-BLOCK_SIZE, self->items_desc);
        if (t == NULL) {
            fprintf(stderr, "bag_remove() error: resizing failed\n");
            return false;
        }
        self->items = t;
        self->cap -= BLOCK_SIZE;
    }
    return true;
}



