#ifndef BIGINT_H
#include <locust/type.bigint.h>
#endif

TYPE_ID_DEF(bigint);

META_FIELDS_DEF(bigint) = {
    2, {
        {TYPE_ID_OF(uint64), sizeof(uint64), "info"},
        {TYPE_ID_OF(ptr), sizeof(uint64*), "bits"},
    }
};

META_DEF(bigint, & META_FIELDS_OF(bigint), nil);

#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#define set_bit(mem, to, mask) (mem ^= (-to ^ mem) & mask)
#define INFO_NEG_BITMASK (1ULL << 63)
static inline void bigint_set_negative(bigint *self, bool is_neg) {
    set_bit(self->info, is_neg, INFO_NEG_BITMASK);
}

static inline void bigint_set_len(bigint *self, uint64 new_len) {
    set_bit(new_len, bigint_is_negative(self), INFO_NEG_BITMASK);
    self->info &= new_len;
}
#undef INFO_NEG_BITMASK
#undef set_bit

void bigint_init(bigint *self, uint64 len) {
    assert(self != nil);
    uint64 *bits = mem_get(& META_OF(uint64), len);
    assert(bits != nil);
    self->info = 0;
    bigint_set_len(self, len);
    self->bits = bits;
}

void bigint_deinit(bigint *self) {
    assert(self != nil);
    mem_free(self->bits);
}

static inline int8 bigint__abs_cmp(bigint self, bigint other) {
    uint64 len1 = bigint_bits_len(&self);
    uint64 len2 = bigint_bits_len(&other);
    if (len1 != len2) {
        return (len1 > len2) ? 1 : -1;
    }

    for (; len1 > 0; --len1) {
        if (self.bits[len1] != other.bits[len1]) {
            return (self.bits[len1] > other.bits[len1]) ?  1 : -1;
        }
    }

    return 0;
}

static inline int8 bigint__cmp(bigint self, bigint other) {
    bool n1 = bigint_is_negative(&self);
    bool n2 = bigint_is_negative(&other);

    if (n1 != n2) {
        return (n1 > n2) ? 1 : -1;
    }

    if (n1) {
        // swap because both are negative
        return bigint__abs_cmp(other, self);
    }

    // both positive
    return bigint__abs_cmp(self, other);
}

#define max_min(a, b, max_dest, min_dest) do { if (a >= b) { max_dest = a; min_dest = b; } else { max_dest = b; min_dest = a; } } while(0)

// assumes dest is the right size
static inline void overloaded bigint__abs_add(bigint *self, bigint *other, bigint *dest) {
    uint64 len1 = bigint_bits_len(self);
    uint64 len2 = bigint_bits_len(other);

    uint64 max_len, min_len;
    max_min(len1, len2, max_len, min_len);

    uint64 i;
    uint128 r;
    for (i=0, r=0; i < min_len; ++i) {
        r += (uint128)self->bits[i] + other->bits[i];
        dest->bits[i] = r;
        r >>= 64;
    }

    if (r > 0) {
        dest->bits[i] = r;
    }
}

// assumes dest is the right size
static inline void overloaded bigint__abs_add(bigint *self, uint64 other, bigint *dest) {
    uint64 len = bigint_bits_len(self);
    // can assume bigint has max len, since it's at least same as uint64
    // can assume uint64 has least len, since it's at mose same as bigint

    uint64 i = 0;
    uint128 r = other;
    for (; i < len && r != 0; ++i) {
        r += (uint128)self->bits[i];
        dest->bits[i] = r;
        r >>= 64;
    }

    if (r > 0) {
        // we hit the end and still have a bit left over
        dest->bits[i] = r;
    } else {
        // determine how much remaining is straight copy
        // note: r goes from "result" to "remaining" in meaning
        r = 8*(len-i);
        if (r > 0) {
            mem_copy(self->bits+i, dest->bits+i, r);
        }
    }
}



/*
bigint *dest = bigint_create(max_len);

mem_resize(dest->bits, & META_OF(uint64), max_len+1);
bigint_set_len(max_len+1);
*/




void overloaded bigint_sub(bigint *self, bigint *other);
void overloaded bigint_mul(bigint *self, bigint *other);
void overloaded bigint_div(bigint *self, bigint *other);

