#ifndef TYPES_H
#include <locust/type.types.h>
#endif

meta const * meta_ptrs = 0;
uint16 meta_max = 0;

META_FIELDS_DEF(meta) = {
    6, {
    	META_FIELD_DEF(meta, name, ptr),
    	META_FIELD_DEF(meta, fields, ptr),
    	META_FIELD_DEF(meta, methods, ptr),
        META_FIELD_DEF(meta, type, type_id),
        META_FIELD_DEF(meta, size, size_t),
        META_FIELD_DEF(meta, size_varies, bool)
    }
};

META_DEF(meta, & META_FIELDS_OF(meta), nil);
