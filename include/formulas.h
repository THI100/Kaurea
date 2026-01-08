#ifndef FORMULAS_H
#define FORMULAS_H

// 32-bit functions
void omega_fuse(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);
uint32_t golden_mix(uint32_t x, uint32_t y, uint32_t z);
uint32_t pi_permutation(uint32_t x, uint32_t r);
uint32_t arxl(uint32_t x, uint32_t c);
uint32_t srperm(uint32_t x);
void minimix(uint32_t *a, uint32_t *b);

// 8-bit functions
static inline uint8_t ROTLB (uint8_t x, uint8_t n);
static inline uint8_t ROTRB (uint8_t x, uint8_t n);
uint8_t arxlB(uint8_t x, uint8_t c);

#endif //FORMULAS_H