#define ERROR_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

typedef struct {
    uint64 id;
    uint64 error_code;
    ptr name;
    ptr info;
} error;

#define error__type_id ((type_id const)20)
META_DECL(error);

extern error e_none;



bool static inline error_equals(error *e1, error *e2) {
    return (e1->id == e2->id);
}

error static inline error_create(uint32 id, ptr name, ptr info) {
    error e = { id, name, info };
    return e;
}

bool static inline error_is_none(error *e) {
    return (e->id == 0 && e->name == 0 && e->info == 0);
}

