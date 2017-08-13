#define BIGINT_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

typedef struct {
    byte bytes[];
} bigint;

#define bigint__type_id ((type_id const)37)
META_DECL(bigint);

void bigint_init(bigint *self, uint64 len);
void bigint_deinit(bigint *self);

//  7 = 1XXXXXXX
// 14 = 0XXXXXXX 1XXXXXXX
// 29 = 0XXXXXXX 0XXXXXXX 1XXXXXXX XXXXXXXX
// 60 = 0XXXXXXX 0XXXXXXX 0XXXXXXX XXXXXXXX 1XXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX
// 123= 0XXXXXXX 0XXXXXXX 0XXXXXXX XXXXXXXX 0XXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX 1XXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX

uint64
static inline nonnullfunc purestfunc
bigint_mem_len(size_t type_size, uint64 count) {
    count = type_size * count;
    count += sizeof(meta const * const) + sizeof(uint64) + sizeof(ptr);
    return count;
}

ptr
static inline nonnullfunc purestfunc
bigint_mem_front(array self) {
	return self - (sizeof(uint64) + sizeof(meta const *));
}

bigint
static inline nonnullfunc
bigint_init(bigint restrict self, uint64 count) {
    *((meta const **)self) = type;
	self += sizeof(meta const * const);
	*((uint64*)self) = count;
	self += sizeof(uint64);
	*((ptr*)(self+count)) = self;
	return self;
}

#define T_BITMASK (1ULL << 63)
#define PTR_N_BITMASK (1ULL)
#define PTR_LEN_BITMASK (3ULL)

uint64
static inline purestfunc
round_up_to_pow2(uint64 x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return ++x;
}

uint64
static inline purestfunc
mem_alloc_length_hint(uint64 current_length) {

}

bigint
static inline nonnullfunc
bigint_create(uint64 length_hint, bool is_negative) {
	// resolved bigints look like a packed
    // typedef struct {
    //  -> byte data[n2];
    // } bigint;
    // where n1 is the length of the the metadata in bytes
    // where n2 is the length of the bigint number in bytes
    //
    // The data looks like, in bits and pseudo ebnf:
	// NXXX XXXX XXXX XXXX   TXXX XXXX XXXX XXXX [, 32 bits with leading T bit {, 64 bits with leading T bit }]]
	// and T is a bit that is true for a terminating "part"
	// and X bits are all data of the number

	// N00 >> 64
	// N01 >> 128
	// N10 >> 64
	// N11 >> >= 128

	length_hint = round_up_to_pow2(length_hint);

	ptr ret = mem_get(bigint_mem_len(type->size, count), & META_OF(bigint));
    if (ret == nil) { error("NilPtr: ret; mem_get() failure"); goto error; }
	return bigint_init(ret, count);
  error:
  	return nil;
}

static inline bool bigint_is_negative(bigint *self) {
    return (self->info & INFO_NEG_BITMASK) > 0;
}

static inline uint64 bigint_bits_len(bigint *self) {
    return self->info & ~INFO_NEG_BITMASK;
}

static inline void bigint_negate(bigint *self) {
    self->info ^= INFO_NEG_BITMASK;
}

#undef T_BITMASK
#undef PTR_N_BITMASK
#undef PTR_LEN_BITMASK
