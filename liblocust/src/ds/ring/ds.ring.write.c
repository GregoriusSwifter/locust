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
ring_write_single(ring * restrict self, ptr restrict item) {
    uint64 front, back, index;
#if !nonnullfunc_on
    assert(self != nil);
    assert(item != nil);
#endif
    size_t item_size = self->a.type->size;

    front = self->wa_counter; // These could increment after this but only the
    back = self->r_counter;  // a_counter is bad to have increment until the cas

    // Check for butting up against the back from other side.
    // If so, not necessarily full at this moment in time, but close enough.
    if (front - back == self->a.length) { return false; }

    index = (self->wa_counter++) & (self->a.length-1);

    // Guaranteed to have acquired a spot in the ring. Shunt item in.
    mem_copy(item, self->a.items+(index*item_size), self->a.type->size);

    // Update the other side once we've written.
    ++(self->w_counter);
    return true;
}

static inline nonnullfunc bool
ring_write_multi(ring * restrict self, ptr restrict item) {
    uint64 front, back, index;
#if !nonnullfunc_on
    assert(self != nil);
    assert(item != nil);
#endif
    uint8 wait_counter = 0;
    size_t item_size = self->a.type->size;

    // This loop either adds the item, or hits a full ring buffer.
    while (true) {
        front = self->wa_counter; // These could increment after this but only the
        back = self->r_counter;  // a_counter is bad to have increment until the cas

        // Check for butting up against the back from other side.
        // If so, not necessarily full at this moment in time, but close enough.
        if (front - back == self->a.length) { return false; }

        // Attempt to acquire a spot in the ring.
        // If not, start this whole attempt over with new spot to acquire.
        if (!atomic_cmp_and_set(&self->wa_counter, front, front+1)) { continue; }

        // Guaranteed to have acquired a spot in the ring. Shunt item in.
        index = front & (self->a.length-1);
        mem_copy(item, self->a.items+(index*item_size), item_size);

        // This increases ring_w_counter only if we're responsible for it.
        while (!atomic_cmp_and_set(&self->w_counter, front, front+1)) {
            wait_gradual(wait_counter++);
        }

        return true;
    }
}

nonnullfunc bool
ring_write(ring * restrict self, ptr restrict item) {
#if !nonnullfunc_on
    assert(self != nil);
    assert(item != nil);
#endif
    // determine usage of ring by number of threads using it.
    switch (self->w_thread_id) {
        case -1ULL: // first usage by any thread. "mark" ring as written by current thread.
            self->w_thread_id = thread_curr.id;
        default: // known to be a single writer so far, check again.
            if (self->w_thread_id != thread_curr.id) {
                // oh shit first usage with multiple writers
                self->w_thread_id = -2ULL;
        case -2ULL: // known to be multiple writers
                return ring_write_multi(self, item);
            }
            // known to be single writer
            return ring_write_single(self, item);
    }
}
