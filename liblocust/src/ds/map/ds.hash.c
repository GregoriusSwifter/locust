#ifndef HASH_H
#include <locust/ds.hash.h>
#endif

hash256 map_hash(byte *data, uint64 length) {
    if (data == NULL || length == 0) {
        hash256 hash = {.p0=0,.p1=0,.p2=0,.p3=0};
        return hash;
    }

    uint64 noise = length;
    noise = _rotr64(noise, 29) ^ _rotr64(noise, 47);
    uint64 _noise = ~noise;


    hash256 hash = {
        .p0 = 0xA55AA55AA55AA55AULL ^ noise,
        .p1 = 0x3C3C3C3C3C3C3C3CULL ^ _noise,
        .p2 = 0xB22BB22BB22BB22BULL + noise,
        .p3 = 0x6969696969696969ULL + _noise,
    };

    byte d, _d;
    for(uint64 i=0; i<length; i++) {
        d = data[i];
        _d = ~d;
        hash.p0 ^= _rotr64(hash.p0 + d, hash.p3) ^ _rotr64(hash.p1 + _d, hash.p0) ^ _rotr64(hash.p2 + d, hash.p1) ^ _rotr64(hash.p3 ^ _d, hash.p2);
        hash.p1 ^= _rotr64(hash.p0 + d, hash.p3) ^ _rotr64(hash.p1 + _d, hash.p0) ^ _rotr64(hash.p2 + d, hash.p1) ^ _rotr64(hash.p3 ^ _d, hash.p2);
        hash.p2 ^= _rotr64(hash.p0 + d, hash.p3) ^ _rotr64(hash.p1 + _d, hash.p0) ^ _rotr64(hash.p2 + d, hash.p1) ^ _rotr64(hash.p3 ^ _d, hash.p2);
        hash.p3 ^= _rotr64(hash.p0 + d, hash.p3) ^ _rotr64(hash.p1 + _d, hash.p0) ^ _rotr64(hash.p2 + d, hash.p1) ^ _rotr64(hash.p3 ^ _d, hash.p2);
    }
    return hash;
}
