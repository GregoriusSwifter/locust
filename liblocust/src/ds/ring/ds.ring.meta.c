#ifndef RING_H
#include <locust/ds.ring.h>
#endif

TYPE_ID_DEF(ring);

META_FIELDS_DEF(ring) = {
    7 , {
    	{.name = "r_counter" , .size = sizeof(uint64), .offset = offsetof(ring, r_counter), .type = TYPE_ID_OF(uint64)},
    	{.name = "ra_counter" , .size = sizeof(uint64), .offset = offsetof(ring, ra_counter), .type = TYPE_ID_OF(uint64)},
     	{.name = "r_thread_id" , .size = sizeof(uint64), .offset = offsetof(ring, r_thread_id), .type = TYPE_ID_OF(uint64)},
    	{.name = "w_counter" , .size = sizeof(uint64), .offset = offsetof(ring, w_counter), .type = TYPE_ID_OF(uint64)},
    	{.name = "wa_counter" , .size = sizeof(uint64), .offset = offsetof(ring, wa_counter), .type = TYPE_ID_OF(uint64)},
     	{.name = "w_thread_id" , .size = sizeof(uint64), .offset = offsetof(ring, w_thread_id), .type = TYPE_ID_OF(uint64)},
    	{.name = "a" , .size = sizeof(array), .offset = offsetof(ring, a), .type = TYPE_ID_OF(array)},
    }
};

META_DEF(ring, & META_FIELDS_OF(ring), nil);
