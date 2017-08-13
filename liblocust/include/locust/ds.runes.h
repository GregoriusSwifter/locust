#define RUNES_H

#ifndef TYPES_H
#include <locust/type.types.h>
#endif

META_DECL(rune);

typedef uint64 rune;

META_DECL(runes);

typedef struct {
    uint64 r_len;
    uint64 b_len;
    byte b[];
} runes;

#define runes_init(s) { .length=(sizeof(STRINGIFY(s)) - 1), .type=&META_OF(byte), .bytes=STRINGIFY(s) }

#define rune_bit1 (7)
#define rune_bitx (6)
#define rune_bit2 (5)
#define rune_bit3 (4)
#define rune_bit4 (3)
#define rune_bit5 (2)

#define rune_t1 (((1<<(rune_bit1+1))-1) ^ 0xFF) /* 0000 0000 */
#define rune_tx (((1<<(rune_bitx+1))-1) ^ 0xFF) /* 1000 0000 */
#define rune_t2 (((1<<(rune_bit2+1))-1) ^ 0xFF) /* 1100 0000 */
#define rune_t3 (((1<<(rune_bit3+1))-1) ^ 0xFF) /* 1110 0000 */
#define rune_t4 (((1<<(rune_bit4+1))-1) ^ 0xFF) /* 1111 0000 */
#define rune_t5 (((1<<(rune_bit5+1))-1) ^ 0xFF) /* 1111 1000 */

#define rune_r1 ((1<<(rune_bit1+0*rune_bitx))-1) /* 0000 0000 0000 0000 0111 1111 */
#define rune_r2 ((1<<(rune_bit2+1*rune_bitx))-1) /* 0000 0000 0000 0111 1111 1111 */
#define rune_r3 ((1<<(rune_bit3+2*rune_bitx))-1) /* 0000 0000 1111 1111 1111 1111 */
#define rune_r4 ((1<<(rune_bit4+3*rune_bitx))-1) /* 0011 1111 1111 1111 1111 1111 */

#define rune_maskx ((1<<rune_bitx)-1)  /* 0011 1111 */
#define rune_testx (rune_maskx ^ 0xFF) /* 1100 0000 */

#define rune_len_max (4)
#define rune_sync ((rune)0x80)
#define rune_self ((rune)0x80)
#define rune_max ((rune)0x10FFFF)
#define rune_err ((rune)0xFFFD)



int32 runes_conv_len(rune *r) {
    uint64 len;
    if (r == nil || 0 == (len = r->length)) {
        return 0;
    }

    bytes *b = &r->bytes[0];
    int32 nb, c;

    nb = 0;
    while(length--) {
        c = *++;
        if(c <= rune_r1)
            nb++;
        else
        if(c <= rune_r2)
            nb += 2;
        else
        if(c <= rune_r3 || c > rune_max)
            nb += 3;
        else
            nb += 4;
    }
    return nb;
}

bool rune_fits(byte *bytes, uint64 n) {
    // Returns 1 if the bytes of length n is long enough to be decoded by chartorune and 0 otherwise.
    // This does not guarantee that the string contains a legal UTF encoding.
    // This routine is used by programs that obtain input a byte at a time & need to know when a full rune has arrived.
    rune r;

    if (n == 0) {
        return 0;
    }
    r = *((byte*)bytes);
    if (r < rune_tx) {
        return 1;
    }
    if (r < rune_t3) {
        return n >= 2;
    }
    if (r < rune_t4) {
        return n >= 3;
    }
    return n >= 4;
}

rune* runes_copy(rune *from) {
    runes(from->bytes)
    mem_copy(from, to, '\0', e - to);
    if(end == nil){
        end = e-1;
        while(end>to && (*--end&0xC0)==0x80)
            ;
        *end = '\0';
    }else{
        end--;
    }
    return end;
}


int runetochar(char *, rune const *);
char *utfrune(char const *, rune);
char *utfrrune(char const *, rune);
char *utfutf(char const *, char const *);

int rune_is_alpha(rune);
int rune_is_digit(rune);
int rune_is_space(rune);

static inline bool rune_is_alnum(rune r) {
    return (bool)(rune_is_alpha(r) || rune_is_digit(r));
}

static inline bool rune_is_blank(rune r) {
    return (bool)(r == ' ' || r == '\t');
}

int rune_is_cntrl(rune r);

static inline bool rune_is_print(rune r) {
    return (bool)(!rune_is_cntrl(r) && r != 0x2028 && r != 0x2029 && !(r >= 0xFFF9 && r <= 0xFFFB));
}

static inline bool rune_is_graph(rune r) {
    return (bool)(!rune_is_space(r) && rune_is_print(r));
}


static inline bool rune_is_punct(rune r) {
    return (bool)(rune_is_graph(r) && !rune_is_alnum(r));
}

int rune_is_lower(rune);



int rune_is_title(rune);
int rune_is_upper(rune);

static inline bool rune_is_valid(rune r) {
    if ((r < 0) || // negative value
        (r >= 0xD800 && r <= 0xDFFF) || // surrogate pair range
        (r >= 0xFDD0 && r <= 0xFDEF) || // non-character range
        ((r & 0xFFFE) == 0xFFFE) || // non-character at end of plane
        (r > 0x10FFFF)) { // too large, thanks to UTF-16
        return 0;
    }
    return 1;
}

static inline bool rune_is_xdigit(rune r) {
    return (bool)((r >= '0' && r <= '9') || (r >= 'A' && r <= 'F') || (r >= 'a' && r <= 'f'));
}

rune rune_to_lower(rune);
rune rune_to_upper(rune);

rune *rune_strcat(rune *, rune const *);
rune *rune_strncat(rune *, rune const *, size_t);
int rune_strcmp(rune const *, rune const *);
int rune_strncmp(rune const *, rune const *, size_t);
rune *rune_strcpy(rune *, rune const *);
rune *rune_strncpy(rune *, rune const *, size_t);
size_t rune_strlen(rune const *);
rune *rune_strchr(rune const *, rune);
rune *rune_strrchr(rune const *, rune);
rune *rune_strdup(rune const *);
rune *rune_strstr(rune const *, rune const *);

extern const unsigned char utftab[64];

