#ifndef MEM_H
#include <locust/rt.mem.h>
#endif

#ifndef DEBUG_H
#include <locust/rt.debug.h>
#endif

#include <syscalls/syscalls.h>

// lookups for random memory location to type therein, at highest level
typedef struct {
    uintptr_t start;
    size_t len;
    int8 type;
} mem_range_desc;

/*
typedef struct {
    void *prev_phys;
    void *prev_free;
    void *next_free;
    uint32 size;
} free_block;

typedef struct {
    void *prev_phys;
    void *next_phys;
    type_desc *type;
    uint32 size;
} used_block;

#define BLOCK_SIZE 4096

void* block_get() {
    void *p;
    uint32 retries = RETRY_COUNT;
    while (retries-->0) {
        p = mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
        if (p == MAP_FAILED) {
            fprintf(stderr, "block_get() error: %s", strerror(errno));
        }
    }
    return (p == MAP_FAILED) ? NULL : p;
}

void block_release(void *p) {
    if (munmap(p, BLOCK_SIZE) != 0) {
        fprintf(stderr, "block_release() error: %s", strerror(errno));
    }
    return;
}


typedef struct {
    uint32 len;
    void *frees;
} free_list;

typedef struct {
    uint64 free_mask;
    free_list *free_lists;
} size_list;


void mem_init() {


}
*/

#include <stdlib.h>
#include <string.h>
#define BLOCK_ITEMS 16 /* largest alloc 262272 bytes */

/* graphitemaster -- #osdev
 * This is a blockade allocator, a new idea in the world of allocators for small short
 * lived objects.  This allocator creates a table of BLOCK_ITEM chunks, which all contain
 * pieces of memory that are allocated for N instances (by blockade_init(N)).  Unlike a
 * traditional block allocator we use integer log base 2 to map the byte-size to a linear
 * sequence: e.g
 *  8  bytes = 0
 *  16 bytes = 1
 *  32 bytes = 2
 *  64 bytes = 3 (etc etc)
 *
 * With the correct mapping we can index into the table with O(1) complexity.  Giving us the
 * memory required, then incrementing the current position to allow us to get the next item
 * in the list for that byte size.
 *
 * On free we simply copy the address of the pointer into a table with the same layout as the
 * chunk table.  Using this table (also called a freelist) the allocator can determine if something
 * is freed and reuse the existing memory.  Since everything works like a basic stack it's easy to
 * branchlessly allocate and deallocate.
 *
 * We use some optimizations like masking pointers so they're either 0 or (cur-last), so when we
 * simply add last you either get cur or last (without a branch).  Some other optimizations the
 * allocator implements is absolute inlinability of integer log base 2 calculation (using a fast
 * dejbruin lookup sequence) and some bitwise tricks (via a macro). We also round up all
 * allocations to the next power of two using a macro.
 *
 * With everything implemented the way it is we benefit from O(1) in both time and space complexity
 * for both allocations and free's (entirely branchless). The cost for initialization however is
 * O(log(N)).  The allocator destruction is also only O(1)
 *
 * The allocator performs a single allocation for everything so all data is contiguous as possible
 * for cache friendliness.  There is no gaps in-between allocations (other than the tiny int before
 * the data to store the size).  There is also no holes in allocations either, we make use of the
 * space efficiently.
 *
 * Traditionally speaking I've never seen anything like this before.  If something has been done
 * of this nature I'd like to know.  But for the most part this is MY invention (since freethinking
 * is what allowed me to vision it).
 */

#define SIZE_JMP2DATA(X)   ((char*)(X) + sizeof(int))
#define SIZE_DEC2DATA(X)   ((char*)(X) - sizeof(int))
#define SIZE_GETAFTER(X)   *((int*)(SIZE_DEC2DATA(X)))
#define SIZE_GETLATER(X)   *((int*)(X))
#define LOGBASE2(X)        blockade_logtable[((size_t)((X) * 0x077CB531U) >> 0x1B) & 0x1F] - 3; /* minus three since we map from 8 not 4 */
#define ROUNDPOT(X)        X--,X|=X>>1,X|=X>>2,X|=X>>4,X|=X>>8,X|=X>>16,X|=X>>32,X++
#define MAPMEM(T,X)       (T)raw; raw += X
#define MAKEIMPL(X)       ((blockade_i*)(X))
#define MAKETYPE(X)       ((blockade_t*)(X))
/* __builtin_ctz replacement table for integer log base 2*/
static const int32 blockade_logtable[0x20] = {
    0x00, 0x01, 0x1C, 0x02, 0x0D, 0x0E, 0x18, 0x03,
    0x1E, 0x16, 0x14, 0x0F, 0x19, 0x11, 0x04, 0x08,
    0x1F, 0x1B, 0x0D, 0x17, 0x15, 0x13, 0x10, 0x07,
    0x1A, 0x0C, 0x12, 0x06, 0x0B, 0x05, 0x0A, 0x09
};

/*
typedef struct {
    void **blocks[BLOCK_ITEMS];
    void **freeme[BLOCK_ITEMS];
    size_t chunks[BLOCK_ITEMS];
    size_t indexs[BLOCK_ITEMS];
    size_t pieces;
    void  *buffer;
} blockade;
*/

blockade *blockade_init(size_t instances) {
    size_t      i,j;
    size_t      bytes = 4;
    char       *raw   = NULL;
    blockade_i *b     = NULL;

    /* Round instances to POT to keep malloc happy (could get lucky and hit an alignment :3) */
    ROUNDPOT(instances);

    if (!(raw = (char*)malloc(sizeof(blockade_i) + (((sizeof(void*) * instances) << 1) * BLOCK_ITEMS) + ((((8 << (BLOCK_ITEMS + 1)) - 1) & ~7) * instances))))
        error("\n:[blockade] failed to allocate memory\n");

    b = MAPMEM(blockade_i*, sizeof(blockade_i));
    b->pieces = instances;
    b->buffer = &raw[-sizeof(blockade_i)]; /* -sizeof(blockade_i) because MAPMEM increments raw */

    /* Map memory for pointers to memory for blocks */
    for (i = 0; i < BLOCK_ITEMS; i++, bytes <<= 1) {
        b->blocks[i] = MAPMEM(void** ,sizeof(void*) * instances);
        b->freeme[i] = MAPMEM(void**, sizeof(void*) * instances);
        /* Map memory to blocks in current item */
        for (j = 0; j < instances; j++)
            b->blocks[i][j] = MAPMEM(void*, bytes);
    }
    memset(
        /* skip blocks and freeme */
        ((char*)b) + ((sizeof(void**) * BLOCK_ITEMS) << 1),
        /* nullify */
        0,
        /* calculate size of:
         *  size_t chunks[BLOCK_ITEMS];
         *  size_t indexs[BLOCK_ITEMS];
         *  size_t pieces;
         *  void  *buffer;
         */
        ((sizeof(size_t) * BLOCK_ITEMS) << 1) + sizeof(size_t) + sizeof(void*)
    );
    return MAKETYPE(b);
}

void *blockade_alloc(blockade *i, size_t bytes) {
    void  *ret     = NULL;
    void  *chk     = NULL;
    void  *lst     = NULL;
    size_t idx     = 0;
    blockade_i *b  = MAKEIMPL(i);

    /* additional space for size and round to POT */
    bytes += sizeof(int);
    ROUNDPOT(bytes);

    idx    = LOGBASE2(bytes);

    /* STORE for less indirection */
    size_t *indexs = &b->indexs[idx];
    size_t *chunks = &b->chunks[idx];

    lst    = b->freeme[idx][*indexs];
    chk    = b->blocks[idx][*chunks];

    /* branchless masking magic bitchs ;-) */
#if 1
    ret = (((!! lst-1 ) & (((char*)chk)-((char*)lst))) + ((char*)lst));
#else
        /* same as this */
    ret = (lst) ? lst : chk;
#endif

    *indexs -= !!lst;
    *chunks += !!chk;

    SIZE_GETLATER(ret) = bytes;
    return SIZE_JMP2DATA(ret);
}

void blockade_free(blockade *i, void *ptr) {
    blockade *b = MAKEIMPL(i);
    size_t      v = LOGBASE2(SIZE_GETAFTER(ptr));
    b->freeme[v][b->indexs[v]] = SIZE_DEC2DATA(ptr);
    b->indexs[v] ++;
}

void blockade_destroy(blockade *b) {
    free(MAKEIMPL(b)->buffer); /* will destroy b too */
}

#undef SIZE_JMP2DATA
#undef SIZE_DEC2DATA
#undef SIZE_GETAFTER
#undef SIZE_GETLATER
#undef LOGBASE2
#undef ROUNDPOT
#undef MAPMEM
#undef MAKEIMPL
#undef MAKETYPE
#undef BLOCK_ITEMS

/*
int main() {
    blockade_t *b = blockade_init(21); // will round POt to 32
    int *z = blockade_alloc(b, sizeof(int));
    #if 0
    char *p = blockade_alloc(b, 55); // 55 + sizeof(int)[4] = 59 (POT round to 64)
    blockade_free(b, p);
    char *e = blockade_alloc(b, 54); // same as last one (will reuse mem block)
    blockade_free(b, e);
    void *f = blockade_alloc(b, 999); // 1024 alloc
    blockade_free(b, z);
    blockade_free(b, f);
    blockade_destroy(b);
    #endif

    return 0;
}
*/
