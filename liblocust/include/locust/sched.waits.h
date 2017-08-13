#define WAITS_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

void static inline wait_active() {
    __asm__("pause");
}

void wait_gradual(uint8 rate);

