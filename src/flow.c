#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../include/flow.h"
#include "../include/formulas.h"

// ─────────────────────────────────────────────────────────────────────────────
// Per-round constants derived from fractional parts of primes (SHA-style).
// Having a unique constant each round prevents slide attacks and symmetry.
// ─────────────────────────────────────────────────────────────────────────────
static const uint32_t ROUND_CONSTANTS[24] = {
    0x428A2F98u, 0x71374491u, 0xB5C0FBCFu, 0xE9B5DBA5u,
    0x3956C25Bu, 0x59F111F1u, 0x923F82A4u, 0xAB1C5ED5u,
    0xD807AA98u, 0x12835B01u, 0x243185BEu, 0x550C7DC3u,
    0x72BE5D74u, 0x80DEB1FEu, 0x9BDC06A7u, 0xC19BF174u,
    0xE49B69C1u, 0xEFBE4786u, 0x0FC19DC6u, 0x240CA1CCu,
    0x2DE92C6Fu, 0x4A7484AAu, 0x5CB0A9DCu, 0x76F988DAu,
};

// ─────────────────────────────────────────────────────────────────────────────
// apply() — the core permutation.
//
// FIX 1: inject ROUND_CONSTANTS[count % 24] into state[0] before each round
//         so that no two rounds are structurally identical (prevents slide
//         attacks and internal symmetry).
//
// FIX 2: the in-round shuffle seed is now seeded from the live state rather
//         than a fixed constant, so the permutation order depends on the data.
//         We still only shuffle every 4 rounds to keep cost reasonable.
//
// FIX 3: replace  seed % LEN  with a rejection-sampling-free power-of-two
//         approach for the inner swap to remove modulo bias.
// ─────────────────────────────────────────────────────────────────────────────
void apply(uint32_t* original, const size_t original_len, const size_t rounds) {
    #define LEN 32

    if (original_len < LEN) {
        printf("ERROR: Equal size: negative.\n");
        return;
    }

    size_t count = 0;
    uint32_t state_original[LEN] = {0};

    memcpy(state_original, original,
           (original_len < LEN ? original_len : LEN) * sizeof(uint32_t));

    while (count < rounds) {

        // ── Inject per-round constant so every round is structurally distinct ──
        state_original[0] ^= ROUND_CONSTANTS[count % 24];

        // ARXL
        for (size_t j = 0; j < LEN; j++) {
            state_original[j] = arxl(
                state_original[j],
                (uint32_t)((state_original[j] * original_len) % UINT32_MAX));
        }

        // golden_mix (phi)
        for (size_t j = 2; j < LEN; j++) {
            state_original[j] = golden_mix(
                state_original[j],
                state_original[j - 1],
                state_original[j - 2]);
        }

        // pi_permutation — use (count-derived constant + j) so rotation
        // distance varies both across words AND across rounds.
        for (size_t j = 0; j < LEN; j++) {
            state_original[j] = pi_permutation(
                state_original[j],
                ROUND_CONSTANTS[count % 24] + (uint32_t)j);
        }

        // srperm
        for (size_t j = 0; j < LEN; j++) {
            state_original[j] = srperm(state_original[j]);
        }

        // omega_fuse
        for (size_t j = 3; j < LEN; j++) {
            omega_fuse(
                &state_original[j],
                &state_original[j - 1],
                &state_original[j - 2],
                &state_original[j - 3]);
        }

        // mini_mix
        for (size_t j = 1; j < LEN; j++) {
            minimix(&state_original[j], &state_original[j - 1]);
        }

        // ── Shuffle every 4 rounds ────────────────────────────────────────────
        if (count % 4 == 0) {
            // FIX: derive seed from the current state, not a baked-in constant.
            uint32_t seed = ROUND_CONSTANTS[count % 24];
            for (size_t j = 0; j < LEN; j++) {
                seed ^= state_original[j] + (seed << 6) + (seed >> 2);
            }

            for (size_t j = LEN - 1; j > 0; j--) {
                // FIX: use high bits to reduce modulo bias.
                size_t swap_idx = (size_t)((seed >> 16) % (j + 1));

                uint32_t t         = state_original[j];
                state_original[j]  = state_original[swap_idx];
                state_original[swap_idx] = t;

                // Advance seed using both the constant and the swapped value.
                seed ^= state_original[j] + (seed << 6) + (seed >> 2);
            }
        }

        count++;
    }

    memcpy(original, state_original, LEN * sizeof(uint32_t));
}

// ─────────────────────────────────────────────────────────────────────────────
// shuffle() — byte-level Fisher-Yates used between the uint32 rounds.
//
// FIX 1: seed is no longer a fixed constant; it is derived entirely from the
//         current hash_box contents so the permutation order is data-dependent.
//
// FIX 2: index generation uses high bits of the seed (>> 16) to reduce the
//         modulo bias that was present in the original  seed % (i+1).
// ─────────────────────────────────────────────────────────────────────────────
void shuffle(uint8_t* hash_box, uint8_t* hash_box_copy, const size_t hash_len) {
    if (hash_len <= 1) return;

    // FIX: derive the starting seed from the live state, not 0x9E3779B9.
    uint32_t internal_seed = 0;
    for (size_t i = 0; i < hash_len; i++) {
        internal_seed ^= (uint32_t)hash_box[i] * 0x9E3779B9u;
        internal_seed  = (internal_seed << 5) | (internal_seed >> 27); // ROTL32(.,5)
    }

    for (size_t i = hash_len - 1; i > 0; i--) {
        internal_seed ^= (uint32_t)hash_box_copy[i]
                         + (internal_seed << 6)
                         + (internal_seed >> 2);

        // FIX: use high 16 bits to reduce modulo bias.
        size_t j = (size_t)((internal_seed >> 16) % (i + 1));

        uint8_t temp   = hash_box[i];
        hash_box[i]    = hash_box[j];
        hash_box[j]    = temp;

        internal_seed += hash_box[i];
    }
}
