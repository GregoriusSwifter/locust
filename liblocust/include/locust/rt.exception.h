#define EXCEPTION_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

#define raise(ex_id) goto CONCAT(__loc_ex, ( ex_id ))
#define catch(ex_id)            \
CONCAT(__loc_ex, ( ex_id )):    \
                                \
