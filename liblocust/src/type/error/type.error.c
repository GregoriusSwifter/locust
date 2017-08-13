#ifndef ERROR_H
#include <locust/type.error.h>
#endif

TYPE_ID_DEF(error);

META_FIELDS_DEF(error) = {
    4, {
        {TYPE_ID_OF(uint64), sizeof(uint64), "id"},
        {TYPE_ID_OF(uint64), sizeof(uint64), "error_code"},
        {TYPE_ID_OF(ptr), sizeof(ptr), "name"},
        {TYPE_ID_OF(ptr), sizeof(ptr), "info"},
    }
};

META_DEF(error, & META_FIELDS_OF(error), nil);

error e_none = { .id=0, .error_code=0, .name=0, .info=0 };
