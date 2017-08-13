#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#include <stdlib.h>

overloaded nonnullfunc mustuseretfunc mallocfunc mallocsizefunc(1) ptr
mem_get(uint64 len, meta const * const m) {
	ptr p = malloc(len);
    if (nil == p) { error("OutOfMemory, malloc() failed"); goto error; }
	return p;
  error:
    return p;
}