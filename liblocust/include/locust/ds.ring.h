#define RING_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef WAITS_H
#include <locust/sched.waits.h>
#endif

#ifndef ARRAY_H
#include <locust/ds.array.h>
#endif

#define ring__type_id ((type_id const)30)
META_DECL(ring);

typedef struct {
    uint64 r_counter;  // read
    uint64 ra_counter; // read acquire -- readers use to claim space. they update r_counter when done.
    uint64 r_thread_id;// used to ensure XPSC OR XPMC usage
    uint64 w_counter;  // write
    uint64 wa_counter; // write acquire -- writers use to claim space. they update w_counter when done.
    uint64 w_thread_id;// used to ensure SPXC or MPXC usage
    array a;
} ring;

bool nonnullfunc ring_write(ring * restrict self, ptr restrict item);
bool nonnullfunc ring_read(ring * restrict self, ptr restrict item);

static inline nonnullfunc void
ring_init(ring *self, meta const * const type, uint64 count) {
#if !nonnullfunc_on
    if (self == nil) { error("NilPtr: self; failure"); goto error; }
    if (type == nil) { error("NilPtr: type; failure"); goto error; }
#endif
    self->r_counter = 0;
    self->ra_counter = 0;
    self->r_thread_id = -1ULL;
    self->w_counter = 0;
    self->wa_counter = 0;
    self->w_thread_id = -1ULL;

    array_init(&self->a, type, count);
error:
    return;
}

static inline nonnullfunc ring *
ring_create(meta const * const type, uint64 count) {
#if !nonnullfunc_on
    if (type == nil) { error("NilPtr: type; failure"); goto error; }
#endif
    ring *self = (ring *)mem_get(sizeof(ring)+(count*type->size), & META_OF(ring));
    if (self == nil) { error("NilPtr: ret; mem_get() failure"); goto error; }

    ring_init(self, type, count);

    return self;
  error:
    return nil;
}

static inline nonnullfunc void
ring_deinit(ring *self) {
#if !nonnullfunc_on
    if (self == nil) { error("NilPtr: self; failure"); goto error; }
#endif
    self->r_counter = 0;
    self->ra_counter = 0;
    self->r_thread_id = -1ULL;
    self->w_counter = 0;
    self->wa_counter = 0;
    self->w_thread_id = -1ULL;

    array_deinit(&self->a);
error:
    return;
}

static inline nonnullfunc void
ring_destroy(ring *self) {
#if !nonnullfunc_on
    if (self == nil) { error("NilPtr: self; failure"); goto error; }
#endif
    ring_deinit(self);
    mem_free(self);
error:
    return;
}
