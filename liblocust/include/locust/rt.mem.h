#define MEM_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

static inline nonnullfunc void
mem_cpy(ptr restrict from, ptr restrict to, uint64 len) {
    __asm__ __volatile__("std; rep; movsb" :"=D" (to), "=S" (from) :"0" (((byte*)to)+len), "1" (((byte*)from)+len), "c" (len) :"memory");
    return;
}

//typedef struct {
//    type_id type;
//    iface_id itype;
//    byte data[];
//} var;

static inline nonnullfunc void
mem_set(var val, ptr dest, uint64 count) {
    uint64 size = meta_get(val.type)->size;
    switch(size) {
        case 64:
            __asm__ __volatile__("cld; rep; stosq" :"=D" (dest) :"0" (dest), "a" (val.data), "c" (count) :);
            break;
        case 32:
            __asm__ __volatile__("cld; rep; stosl" :"=D" (dest) :"0" (dest), "a" (val.data), "c" (count) :);
            break;
        case 16:
            __asm__ __volatile__("cld; rep; stosw" :"=D" (dest) :"0" (dest), "a" (val.data), "c" (count) :);
            break;
        case 8:
            __asm__ __volatile__("cld; rep; stosb" :"=D" (dest) :"0" (dest), "a" (val.data), "c" (count) :);
            break;
        default:
            while (count-->0) {
                mem_cpy(val.data, dest, size);
                dest = (void*)(((byte*)dest)+size);
            }
            break;
    }
}

static inline int8 nonnullfunc
mem_cmp(ptr restrict p1, ptr restrict p2, uint64 len) {
    for (uint64 i=0; i < len; ++i) {
        if (((byte const *)p1)[i] < ((byte const *)p2)[i]) {
            return -1;
        } else if (((byte const *)p1)[i] > ((byte const *)p2)[i]) {
            return 1;
        }
    }
    return 0;
}

ptr nonnullfunc mustuseretfunc mallocfunc mallocsizefunc(1) mem_get(uint64 len, type_id const type);
ptr nonnullfunc mustuseretfunc mallocsizefunc(2) mem_resize(ptr p, uint64 len, type_id const type);
ptr nonnullfunc mem_free(ptr p);


#define BLOCK_ITEMS 16
typedef struct {
    void **blocks[BLOCK_ITEMS];
    void **freeme[BLOCK_ITEMS];
    size_t chunks[BLOCK_ITEMS];
    size_t indexs[BLOCK_ITEMS];
    size_t pieces;
    void  *buffer;
} blockade;

blockade *blockade_init(size_t instances);
void *blockade_alloc(blockade *i, size_t bytes);
void blockade_free(blockade *i, void *ptr);
#undef BLOCK_ITEMS
