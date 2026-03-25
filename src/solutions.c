#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "../include/solutions.h"
#include "../include/formulas.h"

uint8_t* salt(uint8_t* input, size_t input_len, size_t salting_rounds, size_t* out_len) {
    if (input == NULL || input_len == 0) return NULL;

    size_t ilp = input_len + 1;
    uint8_t* temp_box = malloc(ilp);
    if (!temp_box) return NULL;
    memcpy(temp_box, input, input_len);

    temp_box[input_len] = (ilp <= 2)
        ? (((temp_box[0] + (uint8_t)salting_rounds) % 255) + 1)
        : 0x00;

    // Initialize salt_box
    uint8_t* salt_box = malloc(salting_rounds);
    if (!salt_box) { free(temp_box); return NULL; }
    size_t current_len = 0;

    // Generate salt via ARX
    while (current_len < salting_rounds) {
        for (size_t i = 0; i < ilp - 1 && current_len < salting_rounds; i++) {
            salt_box[current_len++] = arxlB(temp_box[i], temp_box[i + 1]);
        }
        current_len++;
    }

    // Initialize salted_input
    uint8_t* salted_input = malloc(salting_rounds + ilp);
    if (!salted_input) { free(temp_box); free(salt_box); return NULL; }
    size_t actual_salted_len = 0;

    // Join while ignoring 0x00
    for (size_t i = 0; i < ilp; i++) {
        if (temp_box[i] != 0x00)
            salted_input[actual_salted_len++] = temp_box[i];
    }
    for (size_t i = 0; i < salting_rounds; i++) {
        if (salt_box[i] != 0x00)
            salted_input[actual_salted_len++] = salt_box[i];
    }

    // Simple Shuffle
    if (actual_salted_len > 0) {
        size_t seed = input_len * 32123u + actual_salted_len;
        for (size_t i = 0; i < actual_salted_len; i++) {
            seed = seed * 1103515245u + 12345u;
            size_t j = seed % (i + 1);

            uint8_t t         = salted_input[i];
            salted_input[i]   = salted_input[j];
            salted_input[j]   = t;
        }
    }

    *out_len = actual_salted_len;

    free(temp_box);
    free(salt_box);

    return salted_input;
}

// ─────────────────────────────────────────────────────────────────────────────
// pad_input — Merkle-Damgård style padding.
//
// Appends:  0x80  |  zero bytes  |  8-byte big-endian bit-length
// The padded length is always a multiple of BLOCK bytes.
// Caller must free() the returned buffer.
//
// Without this, two messages that are prefixes of each other can produce
// related internal states (length-extension attack).
// ─────────────────────────────────────────────────────────────────────────────
uint8_t* pad_input(const uint8_t* input, size_t input_len, size_t block_size, size_t* out_len) {
    // We need at least 1 byte for 0x80 and 8 bytes for the length.
    size_t min_padded = input_len + 1 + 8;
    size_t padded_len = ((min_padded + block_size - 1) / block_size) * block_size;

    uint8_t* buf = calloc(padded_len, 1);
    if (!buf) return NULL;

    memcpy(buf, input, input_len);
    buf[input_len] = 0x80;

    // Encode bit-length as big-endian uint64 in the last 8 bytes.
    uint64_t bit_len = (uint64_t)input_len * 8;
    for (int i = 7; i >= 0; i--) {
        buf[padded_len - 8 + i] = (uint8_t)(bit_len & 0xFF);
        bit_len >>= 8;
    }

    *out_len = padded_len;
    return buf;
}

void cof(uint8_t* input, const size_t input_len,
         uint8_t* hash_box, const size_t hash_len) {
    #define ROTLB(x, n) (((x) << (n)) | ((x) >> (8 - (n))))
    #define ROTRB(x, n) (((x) >> (n)) | ((x) << (8 - (n))))

    size_t seed = input_len * 32123u + hash_len;

    /* ── Compression ── */
    if (input_len > hash_len) {
        size_t in_i = 0;

        for (size_t out_i = 0; out_i < hash_len; out_i++) {
            size_t j = (seed + in_i) % input_len;
            size_t k = (seed ^ input[in_i]) % input_len;

            uint8_t x = input[j];
            uint8_t y = input[k];

            uint8_t r = (uint8_t)(x + ROTLB(y, 3));
            r ^= (uint8_t)seed;
            r  = (uint8_t)(r * 0x9E);
            r ^= (r >> 3);

            hash_box[out_i] = r;

            seed = (seed * 33u) ^ r;
            in_i = (in_i + 1) % input_len;
        }
    }

    /* ── Middle-ground ── */
    else if (input_len == hash_len) {
        memcpy(hash_box, input, hash_len);
    }

    /* ── Expansion ── */
    else {
        uint8_t state = (uint8_t)(seed & 0xFF);

        for (size_t i = 0; i < hash_len; i++) {
            uint8_t n = input[i % input_len];

            state ^= (uint8_t)(n + i);
            state  = ROTLB(state, 3);
            state  = (uint8_t)(state * 0x5B);
            state ^= (state >> 2);

            hash_box[i] = state;
        }
    }
}

void disassemble_blocks(uint8_t* input, const size_t input_len,
                        uint32_t* blocks, const size_t blocks_len) {
    size_t max_blocks = input_len / 4;
    size_t iterations = (max_blocks < blocks_len) ? max_blocks : blocks_len;
    memcpy(blocks, input, iterations * sizeof(uint32_t));
}

void assemble_array(uint32_t* blocks, const size_t blocks_len,
                    uint8_t* hash_box, const size_t hash_len) {
    size_t bytes_to_copy = blocks_len * sizeof(uint32_t);
    if (bytes_to_copy > hash_len)
        bytes_to_copy = hash_len;
    memcpy(hash_box, blocks, bytes_to_copy);
}
