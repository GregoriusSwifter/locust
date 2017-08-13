#define TYPES_H

#include <stdint.h>
#include <stddef.h>

#undef true
#define true ((bool)1)
#undef false
#define false ((bool)0)

#define __CONCAT__(A, B) A ## B
#define CONCAT(A, B) __CONCAT__(A,B)
#define __STRINGIFY__(A) #A
#define STRINGIFY(A) __STRINGIFY__(A)

#define unreachable __builtin_unreachable();

#if 1
#define nonnullfunc __attribute__(( nonnull ))
#define nonnullfunc_on 1
#else
#define nonnullfunc
#define nonnullfunc_on 0
#endif

#define nonnullfunc_args(arg1, ...) __attribute__(( nonnull(arg1, ##__VA_ARGS__) ))
#define inlinefunc __attribute__((always_inline))
#if 0
#define mallocsizefunc(arg1) __attribute__(( alloc_size(arg1) ))
#define mallocsizebysizefunc(arg1, arg2) __attribute__(( alloc_size(arg1, arg2) ))
#else
#define mallocsizefunc(arg1)
#define mallocsizebysizefunc(arg1, arg2)
#endif
#define mallocfunc __attribute__(( malloc ))
#define purefunc __attribute__(( pure ))
#define purestfunc __attribute__(( const ))
#define mustuseretfunc __attribute__(( warn_unused_result ))
#define nil ((ptr const)(0))

typedef void     none;
typedef uint8_t  byte;
typedef uint8_t  bool;
typedef int8_t   int8;
typedef uint8_t  uint8;
typedef int16_t  int16;
typedef uint16_t uint16;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef int64_t  int64;
typedef uint64_t uint64;
typedef void *   ptr;
typedef uint16   type_id;
typedef uint16   iface_id;

typedef struct {
    uint16 const len;
    type_id const args[];
} meta_args;

typedef struct {
    type_id const type;
    bool const is_varargs;
    bool const is_restrict;
    bool const is_static;
    bool const has_error;
    meta_args const * const takes;
    meta_args const * const returns;
} meta_func;

typedef struct {
    uint16 const len;
    meta_func const funcs[];
} meta_funcs;

typedef struct {
    type_id const type;
    bool const is_const;
    bool const is_ptr;
    bool const is_const_ptr;
    bool const is_volatile;
    bool const is_atomic;
} meta_field;

typedef struct {
    uint16 const len;
    meta_field const fields[];
} meta_fields;

typedef struct {
    type_id const type;
    uint16 const size;
    meta_funcs const * const funcs;
    meta_fields const * const fields;
} meta;
extern meta const * meta_ptrs;
extern uint16 meta_max;

typedef struct {
    iface_id const itype;
    uint16 const len;
    meta_func const funcs[];
} iface;

typedef struct {
    type_id const type;
    iface_id const itype;
    byte data[];
} var;



#define META_OF(typ) CONCAT(typ, __meta)
#define TYPED_FUNC_ARG(val_arg_id, type_arg_id) __attribute__(( pointer_with_type_tag(locust_, val_arg_id, type_arg_id) ))
#define TYPE_CHECK(meta1, meta2) assert_static(meta1->type == meta2->type)
#define META_DECL(typ) extern meta const META_OF(typ)
#define META_FUNCTIONS_DEF(typ) static meta_funcs const CONCAT(META_OF(typ), _funcs)
#define META_FIELDS_DEF(typ) static meta_fields const CONCAT(META_OF(typ), _fields)
#define META_DEF(typ) META_DEF_(typ, __COUNTER__)
#define META_DEF_(typ, ctr) do {                                                \
meta__loc_ ## ctr :                                                             \
static meta const META_OF(typ) = {                                              \
    .type __attribute__(( type_tag_for_datatype(locust_, ( typ )) )) = ( ctr ), \
    .size = sizeof( typ ),                                                      \
    .fields = & CONCAT(META_OF(typ), __meta_fields),                            \
    .funcs = & CONCAT(META_OF(typ), __meta_funcs),                              \
};                                                                              \
++meta_max;                                                                     \
} while (0)

META_DECL(none);

void meta_init() {
    int i;
    for (i=0; i<meta_max; i++) {
        meta_ptrs[i] = ((meta const *) &&meta__loc_ ## i );
    }
}

meta const * const meta_get(type_id type) {
    if (type >= meta_max) { return &META_OF(none); }
    return &meta_ptrs[type];
}

#define FACE_INSTANCE(data, type, iface_name) { & data, & META_OF(type), & iface_name }
#define FACE_DECL(iface) face const META_OF(iface)
#define FACE_DEF(iface) face const iface_name

META_DECL(type_id);
META_DECL(bool);
META_DECL(byte);
META_DECL(int8);
META_DECL(uint8);
META_DECL(int16);
META_DECL(uint16);
META_DECL(int32);
META_DECL(uint32);
META_DECL(int64);
META_DECL(uint64);
META_DECL(ptr);
META_DECL(meta);
META_DECL(var);
META_DECL(face);
META_DECL(ivar);

#define var_unbox(self, typ, dat) do { \
    ( typ ) = ( self ).type;           \
    ( dat ) = ( self ).data;           \
} while (0)

#define var_box(self, typ, dat) do { \
    var ( self ) = {                 \
        .type=( typ );               \
        .data=( dat );               \
    };                               \
} while (0)

/*
// Range
typedef struct {
    int64 start;
    int64 stop;
    int64 step;
} int_range;

inline void int_range_init(int_range *self, int64 start, int64 stop, int64 step);
#define int_range_deinit() ((void)0)

static inline bool int_range_index(int_range self, int64 i, int64 *target) {
    i = __myabs(i); // This is weird.
    i = self.start + (i*self.step);
    if ((self.step < 0 && i < self.stop) || (self.step > 0 && i > self.stop)) {
        return false;
    }
    *target = i;
    return true;
}
*/
/*
#define bitsizeof(type) (sizeof(type)*0x8) // 0x8 might technically be CHAR_BIT, but I'm lazy and evil.
// We're using exactly sized types already anyway.

#define __myabs(val) ((( val ) < 0) ? (unsigned)(- ( val )) : ( val ) )

static inline int64 __myintabs(int64 val) {
    int64 mask = val >> (bitsizeof(int64) - 1);
    return (val + mask) ^ mask;
}*/

/*
// Slice
// looks like a collection, often refers to a collection, but doesn't own memory.
typedef struct {
    meta const * type;
    ptr object;
    int64 start;
    int64 stop;
    int64 step;
} slice;


META_DECL(slice);

static inline nonnullfunc void
slice_init(slice *self, meta const * const type, ptr restrict object, int64 start, int64 stop, int64 step) {
    self->type = type;
    self->object = object;
    self->start = start;
    self->stop = stop;
    self->step = step;
    return;
}
*/
/*
#define rotate_right(value, bits) _Generic(__typeof__(value), \
  uint8_t: _rotr8(value, bits),                             \
  uint16_t: _rotr16(value, bits),                           \
  uint32_t: _rotr32(value, bits),                           \
  default: _rotr64(value, bits))

#define rotate_left(value, bits) _Generic(__typeof__(value), \
  uint8_t: _rotl8(value, bits),                            \
  uint16_t: _rotl16(value, bits),                          \
  uint32_t: _rotl32(value, bits),                          \
  default: _rotl64(value, bits))

uint8_t static inline _rotl8(uint8_t value, uint8_t count) {
    count &= 7;
    return (value<<count) | (value>>( (-count) & 7 ));
}

uint16_t static inline _rotl16(uint16_t value, uint8_t count) {
    count &= 15;
    return (value<<count) | (value>>( (-count) & 15 ));
}

uint32_t static inline _rotl32(uint32_t value, uint8_t count) {
    count &= 31;
    return (value<<count) | (value>>( (-count) & 31 ));
}

uint64_t static inline _rotl64(uint64_t value, uint8_t count) {
    count &= 63;
    return (value<<count) | (value>>( (-count) & 63 ));
}

uint8_t static inline _rotr8(uint8_t value, uint8_t count) {
    count &= 7;
    return (value>>count) | (value<<( (-count) & 7 ));
}

uint16_t static inline _rotr16(uint16_t value, uint8_t count) {
    count &= 15;
    return (value>>count) | (value<<( (-count) & 15 ));
}

uint32_t static inline _rotr32(uint32_t value, uint8_t count) {
    count &= 31;
    return (value>>count) | (value<<( (-count) & 31 ));
}

uint64_t static inline _rotr64(uint64_t value, uint8_t count) {
    count &= 63;
    return (value>>count) | (value<<( (-count) & 63 ));
}


static inline uint64 min(uint64 val1, uint64 val2) {
    return val2 ^ ((val1 ^ val2) & -(val1 < val2));
}

static inline uint64 max(uint64 val1, uint64 val2) {
    return val1 ^ ((val1 ^ val2) & -(val1 < val2));
}
*/
