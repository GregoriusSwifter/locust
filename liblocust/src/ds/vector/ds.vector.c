#ifndef VECTOR_H
#include <locust/ds.vector.h>
#endif

TYPE_ID_DEF(vector);

META_FIELDS_DEF(vector) = {
    4, {
        META_FIELD_DEF(vector, items_desc, ptr),
        META_FIELD_DEF(vector, length, uint64),
        META_FIELD_DEF(vector, items, ptr),
    }
};

META_DEF(vector, & META_FIELDS_OF(vector), nil);


