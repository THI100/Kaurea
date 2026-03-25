#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "../include/solutions.h"
#include "../include/flow.h"

// ─────────────────────────────────────────────────────────────────────────────
// hash()
//
// Changes vs. original:
//
//   FIX 1 — Padding before absorption (solutions.c: pad_input).
//            The input is padded with a 0x80 sentinel and a big-endian
//            64-bit bit-length field before being fed to cof().  This
//            means two messages that are prefixes of each other always
//            produce distinct padded byte strings, closing the
//            length-extension attack surface.
//
//   FIX 2 — hash_len is a size_t*, not size_t.
//            The caller can now read back the actual output length
//            (always LIMIT*2+1 = 257 bytes including the NUL terminator).
//            Assigning to a local copy of a value parameter was silent UB
//            that misled callers into thinking the length was communicated.
//
//   FIX 3 — NULL-check moved before the cast of input to uint8_t*.
// ─────────────────────────────────────────────────────────────────────────────

// @param hash_len  Out-parameter; set to the allocated string length on success.
char* hash(const char* input, const size_t input_len,
           const size_t salting_rounds, size_t* hash_len) {
    #define LIMIT 128
    #define BLEN  32
    #define BLOCK_SIZE 64   /* padding block size in bytes */

    // FIX 3: NULL check before anything else.
    if (!input) return NULL;

    uint8_t  hash_box[LIMIT] = {0};
    uint8_t* input_bytes     = (uint8_t*)input;

    // ── Salting ──────────────────────────────────────────────────────────────
    if (salting_rounds > 0) {
        size_t  salted_len = 0;
        uint8_t* temp = salt(input_bytes, input_len, salting_rounds, &salted_len);
        if (!temp) return NULL;

        uint8_t* salted = realloc(temp, salted_len * sizeof(uint8_t));
        if (!salted) { free(temp); return NULL; }

        // FIX 1: pad the salted input before compression.
        size_t  padded_len = 0;
        uint8_t* padded = pad_input(salted, salted_len, BLOCK_SIZE, &padded_len);
        free(salted);
        if (!padded) return NULL;

        cof(padded, padded_len, hash_box, LIMIT);
        free(padded);
    }
    else {
        // FIX 1: pad even when no salting is requested.
        size_t  padded_len = 0;
        uint8_t* padded = pad_input(input_bytes, input_len, BLOCK_SIZE, &padded_len);
        if (!padded) return NULL;

        cof(padded, padded_len, hash_box, LIMIT);
        free(padded);
    }

    // ── Main permutation loop ────────────────────────────────────────────────
    for (size_t i = 0; i < 16; i++) {
        uint32_t blocks[BLEN] = {0};
        disassemble_blocks(hash_box, LIMIT, blocks, BLEN);
        apply(blocks, BLEN, 16);

        uint8_t hash_cpy[LIMIT] = {0};
        memcpy(hash_cpy, hash_box, LIMIT);

        assemble_array(blocks, BLEN, hash_box, LIMIT);
        shuffle(hash_box, hash_cpy, LIMIT);
    }

    // ── Encode as hex string ─────────────────────────────────────────────────
    char* result = malloc(LIMIT * 2 + 1);
    if (!result) return NULL;

    static const char hex_digits[] = "0123456789abcdef";

    for (size_t i = 0; i < LIMIT; i++) {
        result[i * 2]     = hex_digits[hash_box[i] >> 4];
        result[i * 2 + 1] = hex_digits[hash_box[i] & 0x0F];
    }
    result[LIMIT * 2] = '\0';

    // FIX 2: write actual length back through the pointer.
    if (hash_len) *hash_len = LIMIT * 2 + 1;

    return result;   // caller must free()
}
