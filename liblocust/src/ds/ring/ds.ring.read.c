#ifndef RING_H
#include <locust/ds.ring.h>
#endif

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef ATOMIC_H
#include <locust/sched.atomic.h>
#endif

#ifndef WAITS_H
#include <locust/sched.waits.h>
#endif

#ifndef THREADS_H
#include <locust/sched.threads.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

static inline nonnullfunc bool
ring_read_single(ring * restrict self, ptr restrict item) {
    uint64 front, back, index;
#if !nonnullfunc_on
    assert(self != nil);
    assert(item != nil);
#endif
    size_t item_size = self->a.type->size;

    back = self->ra_counter; // These could increment after this but only the
    front = self->w_counter; // r_counter is bad to have increment until the cas

    // Check for butting up against the back from other side.
    // If so, not necessarily full at this moment in time, but close enough.
    if (front - back == 0) { return false; }

    index = (self->ra_counter++) & (self->a.length-1);

    // Guaranteed to have acquired a spot in the ring. Read item out.
    mem_copy(self->a.items+(index*item_size), item, item_size);

    // Update the other side once we've read.
    ++(self->r_counter);
    return true;
}

static inline nonnullfunc bool
ring_read_multi(ring * restrict self, ptr restrict item) {
    uint64 front, back, index;
#if !nonnullfunc_on
    assert(self != nil);
    assert(item != nil);
#endif
    uint8 wait_counter = 0;
    size_t item_size = self->a.type->size;

    while (true) {
        back = self->ra_counter; // These could increment after this but only the
        front = self->w_counter;// r_counter is bad to have increment until the cas

        // Check for butting up against the front.
        // If so, not necessarily empty at this moment in time, but close enough.
        if (front - back == 0) { return false; }

        // Attempt to relinquish a spot in the ring.
        // If not, start this whole attempt over with new spot to relinquish.
        if (!atomic_cmp_and_set(&self->ra_counter, back, back+1)) { continue; }

        // Guaranteed to have acquired a spot in the ring. Read item out.
        index = back & (self->a.length-1);
        mem_copy(self->a.items+(index*item_size), item, item_size);

        // This increases ring_r_counter only if we're responsible for it.
        while (!atomic_cmp_and_set(&self->r_counter, back, back+1)) {
            wait_gradual(wait_counter++);
        }

        return true;
    }
}

nonnullfunc bool
ring_read(ring * restrict self, ptr restrict item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(item != nil);
#endif
    // determine usage of ring by number of threads using it.
    switch (self->r_thread_id) {
        case -1ULL: // first usage by any thread. "mark" ring as read by current thread.
            self->r_thread_id = thread_curr.id;
        default: // known to be a single reader so far, check again.
            if (self->r_thread_id != thread_curr.id) {
                // oh shit first usage with multiple readers
                self->r_thread_id = -2ULL;
        case -2ULL: // known to be multiple readers
                return ring_read_multi(self, item);
            }
            // known to be single reader
            return ring_read_single(self, item);
    }
}