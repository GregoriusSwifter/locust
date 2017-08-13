#ifndef MAP_H
#include <locust/ds.map.h>
#endif

TYPE_ID_DEF(map);

META_FIELDS_DEF(map) = {
    5, {
        META_FIELD_DEF(map, items, ptr),
        META_FIELD_DEF(map, items_desc, ptr),
        META_FIELD_DEF(map, len, uint32),
        META_FIELD_DEF(map, cap, uint32),
        META_FIELD_DEF(map, items_stride, uint32),
    }
};

META_DEF(map, & META_FIELDS_OF(map), nil);

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef HASH_H
#include <locust/ds.hash.h>
#endif

#ifndef ASSERT_H
#include <locust/rt.debug.h>
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define MAP_DEFAULT_CAP 16

nonnullfunc void
map_init(map *self, const meta *type, uint64 reserved) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
#endif
    self->len = 0;
    self->cap = (reserved == 0) ? MAP_DEFAULT_CAP : reserved;
    self->items_stride = sizeof(uint32) + type->size;
    self->items_desc = type;
    self->items = mem_get(reserved*self->items_stride);
    assert(self->items != nil);
    mem_set(self->items, reserved*self->items_stride, 0);
    return;
}

nonnullfunc void
map_deinit(map *self) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    mem_free(self->items);
}


nonnullfunc bool
map_set(map *self, uint32 key, const meta *type, ptr value) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(value != nil);
#endif
    TYPE_CHECK(self->items_desc, type);
    uint32 mask = self->cap-1;
    uint32 key_sized = key & mask;
    uint32 i = key_sized;

    byte *contents = self->items;
    byte *curr;
    uint32 curr_key;

    do {
        curr = contents+(i*self->items_stride);
        curr_key = *(uint32*)curr;


        if (curr_key == key) { return true; }
        if (curr_key == 0) {
            printf("dee1");
			mem_copy(&key, curr, sizeof(uint32));
            printf("dee2");
			mem_copy(value, curr+sizeof(uint32), type->size);
            return true;
        }
        ++i; i &= mask;
    } while (i != key_sized);

    // If we make it this far, something fucked up.
    assert("ERROR: map_set() map is FULL" && false);
    unreachable;
}

nonnullfunc bool
map_get(map *self, uint32 key, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);
    uint32 mask = self->cap-1;
    uint32 key_sized;
    uint32 i;
    i = key_sized = key & mask;

    byte *contents = self->items;
    byte *curr;
    uint32 curr_key;

    do {
        curr = contents+(i*self->items_stride);
        curr_key = *(uint32*)curr;

        if (curr_key == key) {
            mem_copy(curr+sizeof(uint32), item, type->size);
            return true;
        }
        if (curr_key == 0) { break; }
        ++i; i &= mask;
    } while (i != key_sized);

    return false;
}

nonnullfunc bool
map_has(map *self, uint32 key) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    uint32 mask = self->cap-1;
    uint32 key_sized;
    uint32 i;
    i = key_sized = key & mask;

    byte *contents = self->items;
    byte *curr;
    uint32 curr_key;

    do {
        curr = contents+(i*self->items_stride);
        curr_key = *(uint32*)curr;

        if (curr_key == key) { return true; }
        if (curr_key == 0) { break; }
        ++i; i &= mask;
    } while (i != key_sized);

    return false;
}

nonnullfunc bool
map_del(map *self, uint32 key) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    uint32 mask = self->cap-1;
    uint32 key_sized;
    uint32 i;
    i = key_sized = key & mask;

    byte *contents = self->items;
    byte *curr;
    uint32 curr_key;

    do {
        curr = contents+(i*self->items_stride);
        curr_key = *(uint32*)curr;

        if (curr_key == key) {
            mem_set(curr, self->items_stride, 0);
            --self->len;
            return true;
        }
        if (curr_key == 0) { break; }
        ++i; i &= mask;
    } while (i != key_sized);

    return false;
}

nonnullfunc bool
map_pop(map *self, uint32 key, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    TYPE_CHECK(self->items_desc, type);
    uint32 mask = self->cap-1;
    uint32 key_sized;
    uint32 i;
    i = key_sized = key & mask;

    byte *contents = self->items;
    byte *curr;
    uint32 curr_key;

    do {
        curr = contents+(i*self->items_stride);
        curr_key = *(uint32*)curr;

        if (curr_key == key) {
            mem_copy(curr+sizeof(uint32), item, type->size);
            mem_set(curr, self->items_stride, 0);
            --self->len;
            return true;
        }
        if (curr_key == 0) { break; }
        ++i; i &= mask;
    } while (i != key_sized);

    return false;
}
