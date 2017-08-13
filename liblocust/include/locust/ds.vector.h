#define VECTOR_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

// Vector is a lot like C++'s vector in costs. The differences are:
// - insertion is only allowed at the end. (you can still alter values anywhere)
// - atomicity guarantees. :) (which is why the previous happened.)

typedef struct {
    meta const * type;
    uint64 offset;
    uint64 capacity;
    ptr items;
} vector;

#define vector__type_id ((type_id const)28)
META_DECL(vector);

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef ATOMIC_H
#include <locust/sched.atomic.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

void overloaded vector_init(vector *self, const meta *type);
void overloaded vector_init(vector *self, const meta *type, uint64 reserved);
void vector_deinit(vector *self);

bool overloaded nonnullfunc vector_grow(vector *self);
bool overloaded nonnullfunc vector_grow(vector *self, uint64 count);

static inline nonnullfunc bool
__vector_push(vector *self, meta const *type, ptr item) TYPED_FUNC_ARG(3, 2) {
    // Check for needing to allocate initial space or growing
    if (self->length == self->capacity) {
        if (vector_grow(self) == false) { error("vector_push() error: growing failed\n"); goto error; }
    }
    array_grow();
    // Get our index into items
    uint64 index = atomic_fetch_and_add(&self->len, 1);
    // J-j-jam it in.
    mem_copy(item, self->items+(index*self->items_desc->size), self->items_desc->size);
    return true;
  error:
    return false;
}
#define vector_push(vector, type, item) assert_static(( vector ) != NULL && \
                                                    ( type ) != NULL && \
                                                    ( item ) != NULL && \
                                                    ( vector )->type  == ( type )) && \
                                        __vector_push(( vector ), ( type ), ( item ))


static inline nonnullfunc bool
__vector_pop(vector *self, meta const *type, ptr item) TYPED_FUNC_ARG(3, 2) {
    if (self->length < 1) { goto error; }

    uint64 index = atomic_fetch_and_add(&self->len, (int64)-1);
    uint32 stride = self->items_desc->size;
    byte* items_bytes = self->items;
    mem_copy(items_bytes+((index-1)*stride), item, stride);

    // TODO: decide whether or not we shrink ever
    /*
    // Check for possible shrinking.
    if (((self->cap - self->len) > (BLOCK_SIZE * 2))
        && (self->cap > (BLOCK_INITIAL+BLOCK_SIZE))) {
        ptr t = mem_resize(self->items, self->cap-BLOCK_SIZE, self->items_desc);
        if (t == nil) { error("vector_remove() error: resizing failed\n"); goto error; }
        self->items = t;
        self->cap -= BLOCK_SIZE;
    }
    */
    return true;
  error:
    return false;
}

#define vector_pop(vector, type, item) assert_static(( vector ) != NULL && \
                                                   ( type ) != NULL && \
                                                   ( item ) != NULL && \
                                                   ( vector )->type  == ( type )) && \
                                       __vector_set(( vector ), ( type ), ( item ))


static inline nonnullfunc vector *
__vector_resize(vector *self, uint64 count) {
    self = mem_resize(self, sizeof(vector)+array_index(self, count), vector->type);
    if (self == nil) { error("mem_resize() error: resizing failed"); goto error; }
    self->length = count;
error:
    return self;
}
#define vector_resize(vector, count) assert_static(( vector ) != NULL) && \
                                     __vector_resize(( vector ), ( count ))

bool vector_set(vector *self, uint64 index, const meta *type, ptr item);
bool vector_get(vector *self, uint64 index, const meta *type, ptr item);


static inline overloaded nonnullfunc void
__vector_init(vector *self, meta const * const type) {
    self->items_desc = type;
    self->length = 0;
    self->items = nil;
    return;
}
#define vector_init(vector, type) assert_static(( vector ) != NULL && \
                                                ( type ) != NULL) && \
                                  __vector_init(( vector ), ( type ))

static inline overloaded nonnullfunc void
__vector_init(vector *self, meta const * const type, uint64 reserved) {
    self->length = 0;
    self->capacity = type->size * reserved;
    self->items_desc = type;
    self->items = mem_get(self->cap);
    if (self->items == nil) { error("vector_init() error: allocating failed with %lu of type %s\n", reserved, type->name); }
    return;
}
#define vector_init(vector, type, reserved) assert_static(( vector ) != NULL && \
                                                          ( type ) != NULL && \
                                                          ( reserved ) > 0) && \
                                            __vector_init(( vector ), ( type ), ( reserved ))


nonnullfunc void
vector_deinit(vector *self) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    mem_free(self->items);
}

overloaded nonnullfunc bool
vector_grow(vector *self) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    return vector_grow(self, 0);
}

overloaded nonnullfunc bool
vector_grow(vector *self, uint64 count) {
#if !nonnullfunc_on
    assert(self != nil);
#endif
    if (self->cap == 0 || self->items == nil) {
        // initial allocation
        if (count == 0) {
            count = BLOCK_INITIAL;
        }
        self->items = mem_get(count*self->items_desc->size, self->items_desc);
        if (self->items == nil) { error("vector_grow() error: allocating failed\n"); goto error; }
    } else {
        // growing allocation
        if (count == 0) {
            count = BLOCK_SIZE;
        }
        ptr t = mem_resize(self->items, (self->cap+count)*self->items_desc->size, self->items_desc);
        if (t == nil) { error("vector_grow() error: resizing failed\n"); goto error; }
        self->items = t;
    }
    self->cap += count;
    return true;
  error:
    return false;
}

nonnullfunc bool
vector_set(vector *self, uint64 index, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    // Bounds check
    if (index >= self->len) { error("vector_set() error: index out of bounds. %ld >= %ld.\n", index, self->len); goto error; }

    uint32 stride = self->items_desc->size;
    byte* items_bytes = self->items;
    // J-j-jam it in.
    mem_copy(item, items_bytes+(index*stride), stride);
    return true;
  error:
    return false;
}

nonnullfunc bool
vector_push(vector *self, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    // Check for needing to allocate initial space or growing
    if (self->len == self->cap) {
        if (vector_grow(self) == false) { error("vector_push() error: growing failed\n"); goto error; }
    }

    // Get our index into items
    uint64 index = atomic_fetch_and_add(&self->len, 1);
    // J-j-jam it in.
    mem_copy(item, self->items+(index*self->items_desc->size), self->items_desc->size);
    return true;
  error:
    return false;
}

nonnullfunc bool
vector_get(vector *self, uint64 index, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    // Bounds check
    if (index >= self->len) { error("vector_get() error: index out of bounds. %ld >= %ld.\n", index, self->len); goto error; }

    uint32 stride = self->items_desc->size;
    byte* items_bytes = self->items;
    mem_copy(items_bytes+(index*stride), item, stride);
    return true;
  error:
    return false;
}

nonnullfunc bool
vector_pop(vector *self, const meta *type, ptr item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(type != nil);
    assert(item != nil);
#endif
    if (self->len < 1) { goto error; }

    uint64 index = atomic_fetch_and_add(&self->len, (int64)-1);
    uint32 stride = self->items_desc->size;
    byte* items_bytes = self->items;
    mem_copy(items_bytes+((index-1)*stride), item, stride);

    // TODO: decide whether or not we shrink ever
    /*
    // Check for possible shrinking.
    if (((self->cap - self->len) > (BLOCK_SIZE * 2))
        && (self->cap > (BLOCK_INITIAL+BLOCK_SIZE))) {
        ptr t = mem_resize(self->items, self->cap-BLOCK_SIZE, self->items_desc);
        if (t == nil) { error("vector_remove() error: resizing failed\n"); goto error; }
        self->items = t;
        self->cap -= BLOCK_SIZE;
    }
    */
    return true;
  error:
    return false;
}


