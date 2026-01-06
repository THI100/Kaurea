#include <stdint.h>

#include "utils.h"

static inline uint32_t ROTL32(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}

static inline uint32_t ROTR32(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

static inline uint32_t ROTL8(uint32_t x, uint32_t n) {
    n &= 3;
    return (x << (n * 8)) | (x >> (32 - n * 8));
}

static inline uint32_t ROTR8(uint32_t x, uint32_t n) {
    n &= 3;
    return (x >> (n * 8)) | (x << (32 - n * 8));
}