#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../include/flow.h"
#include "../include/formulas.h"

void apply(uint32_t* original, const size_t original_len, const size_t rounds) {
    #define LEN 32

    if (original_len < LEN) {
        printf("ERROR: Equal size: negative.\n");
        return;
    }

    size_t count = 0;
    uint32_t state_original[LEN] = {0};
    
    memcpy(state_original, original, (original_len < LEN ? original_len : LEN) * sizeof(uint32_t));
    
    uint32_t seed = 0x1f2feaff;

    while (count < rounds) {

        // ARXL
        for (size_t j = 0; j < LEN; j++) {
            state_original[j] = arxl(state_original[j], (uint32_t)((state_original[j] * original_len) % UINT32_MAX));
        }

        // phi_Mix: Corrected loop bounds and logic
        for (size_t j = 2; j < LEN; j++) {
            state_original[j] = golden_mix(state_original[j], state_original[j-1], state_original[j-2]);
        }

        // pi_perm: Fixed 'i' to 'count'
        for (size_t j = 0; j < LEN; j++) {
            state_original[j] = pi_permutation(state_original[j], (seed + (uint32_t)(j * count)));
        }

        // srperm
        for (size_t j = 0; j < LEN; j++) {
            state_original[j] = srperm(state_original[j]);
        }

        // omega_fuse
        for (size_t j = 3; j < LEN; j++) {
            omega_fuse(&state_original[j], &state_original[j-1], &state_original[j-2], &state_original[j-3]);
        }

        // mini_mix
        for (size_t j = 1; j < LEN; j++) {
            minimix(&state_original[j], &state_original[j-1]);
        }

        // Shuffle
        if (count % 4 == 0) {
            for (size_t j = 0; j < LEN; j++) {
                seed = seed * (uint32_t)count + (uint32_t)j;
                size_t swap_idx = seed % LEN;

                uint32_t t = state_original[j];
                state_original[j] = state_original[swap_idx];
                state_original[swap_idx] = t;
            }
        }
        
        count++;
    }

    memcpy(original, state_original, LEN * sizeof(uint32_t));
}

void shuffle(uint8_t* hash_box, uint8_t* hash_box_copy, const size_t hash_len) {
    if (hash_len <= 1) return;

    // fixed seed
    uint32_t internal_seed = 0x9E3779B9; 

    for (size_t i = hash_len - 1; i > 0; i--) {
        internal_seed ^= hash_box_copy[i] + (internal_seed << 6) + (internal_seed >> 2);

        // Generate a pseudo-random index based on the seed
        size_t j = internal_seed % (i + 1);

        // Swap elements in hash_box
        uint8_t temp = hash_box[i];
        hash_box[i] = hash_box[j];
        hash_box[j] = temp;
        
        // Update the copy to prevent repetitive patterns
        internal_seed += hash_box[i];
    }
}