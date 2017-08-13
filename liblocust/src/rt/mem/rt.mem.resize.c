#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#include <stdlib.h>

overloaded nonnullfunc mustuseretfunc mallocsizefunc(2) ptr
mem_resize(ptr p, uint64 len, meta const * const m) {
    if (nil == (p = realloc(p, len))) { error("OutOfMemory, realloc() failed"); goto error; }
  error:
    return p;
}