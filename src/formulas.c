#include <stdint.h>

#include "../include/formulas.h"

// Macros for bitwise rotation (32-bit)
#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define ROTLB(x, n) (((x) << (n)) | ((x) >> (8 - (n))))
#define ROTRB(x, n) (((x) >> (n)) | ((x) << (8 - (n))))

// F1: Updates four values in place using pointers
void omega_fuse(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
    *a += *b ^ 0x6A09E667;
    *d ^= ROTL(*a, 9);
    *c += *d;
    *b ^= ROTR(*c, 11);
}

// F2: Standard bitwise mixing
uint32_t golden_mix(uint32_t x, uint32_t y, uint32_t z) {
    uint32_t t = x + y + 0x9E3779B9;
    t ^= ROTL(z, 11);
    t ^= (x & y) ^ (~x & z);
    return ROTR(t, 7);
}

// F3: Permutation with a dynamic shift based on 'r'
uint32_t pi_permutation(uint32_t x, uint32_t r) {
    uint32_t k = (r * 0x243F6A88) & 31;
    return ROTL(x, k) ^ ROTR(x, 32 - k);
}

// F4: Basic ARX transformation
uint32_t arxl(uint32_t x, uint32_t c) {
    x += c;
    x ^= ROTR(x, 11);
    return ROTL(x, 3);
}

// F5: Swaps 16-bit halves then rotates
uint32_t srperm(uint32_t x) {
    x = (x << 16) | (x >> 16);
    return ROTL(x, 9);
}

// F6: Updates two values in place
void minimix(uint32_t *a, uint32_t *b) {
    *a += *b;
    *b ^= ROTL(*a, 7);
}

uint8_t arxlB(uint8_t x, uint8_t c) {
    x += c;
    x ^= ROTRB(x, 5);
    return ROTLB(x, 3);
}