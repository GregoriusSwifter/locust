#ifndef TYPES_H
#include <locust/type.types.h>
#endif


META_FIELDS_DEF(slice) = {
    5, {
    	META_FIELD_DEF(slice, type, ptr),
    	META_FIELD_DEF(slice, object, ptr),
        META_FIELD_DEF(slice, start, int64),
        META_FIELD_DEF(slice, stop, int64),
        META_FIELD_DEF(slice, step, int64)
    }
};

META_DEF(slice);
