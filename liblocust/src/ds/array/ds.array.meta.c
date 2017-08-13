#ifndef ARRAY_H
#include <locust/ds.array.h>
#endif

META_FIELDS_DEF(array) = {
    3, {
    	{.type = TYPE_OF(ptr)},
    	{.type = TYPE_OF(uint64)},
    	{.type = TYPE_OF(ptr)},
    }
};

META_FUNCS_DEF(array) = {
    6, {
        {.name = "array_init", .takes = {3, {TYPE_OF(array ptr), TYPE_OF(meta ptr), TYPE_OF(uint64)}}, .returns = {0, {}}},
        {.name = "array_create", .takes = {2, {TYPE_OF(meta const * const ), TYPE_OF(uint64)}}, .returns = {1, {array TYPE_ID_OF(array*)}}},
        {.name = "", .takes = {0, {}}, .returns = {0, {}}},
    }
};

META_DEF(array);
