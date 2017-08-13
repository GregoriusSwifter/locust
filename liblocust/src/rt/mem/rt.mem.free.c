#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#ifndef THREADS_H
#include <locust/sched.threads.h>
#endif

nonnullfunc ptr
mem_free(ptr p) {
    if (nil == p) { error("NilPtr: mem_free() passed nil failure"); goto error; }
    blockade_free(((blockade_t*)(&thread_curr.mem)), p);
error:
    return nil;
}
