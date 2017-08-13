#define ARRAY_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

META_DECL(array);

typedef struct {
    type_id type;
    uint64 b_len;
    byte b[];
} array;

static inline nonnullfunc void
array_init(array *self, type_id const type, uint64 count) {
    self->type = type;
    self->b_len = count*(meta_get(type)->size);
    return;
}

static inline nonnullfunc array*
array_create(type_id const type, uint64 count) {
	array* ret = mem_get(sizeof(array)+(count*(meta_get(type)->size)), type);
    if (ret == nil) { error("NilPtr: ret; mem_get() failure"); goto error; }
    array_init(ret, type, count);
	return ret;
  error:
  	return nil;
}

static inline nonnullfunc void
array_deinit(array *self) {
    var val = {.type=META_OF(byte), .data=0};
    mem_set(val, self, sizeof(array)+self->b_len);
    return;
}

static inline nonnullfunc void
array_destroy(array* self) {
    array_deinit(self);
    mem_free(self);
    return;
}

static inline nonnullfunc void
array_set(array *self, uint64 index, var item) {
    mem_cpy(item, &(self->b+(i*(meta_get(self->type)->size))), meta_get(self->type)->size);
    return;
}

static inline nonnullfunc void
array_get(array *self, uint64 index, var item) {
    mem_cpy(&(self->b+(i*(meta_get(self->type)->size))), item, meta_get(self->type)->size);
    return;
}
