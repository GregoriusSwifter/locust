#define INSPECT_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

//extern uint32 proc_total;
//extern uint32 proc_active;
//extern uint32 cache_line_size;

uint32 inspect_proc_total();
uint32 inspect_proc_active();
uint32 inspect_cache_line_size();
