#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "../include/kaurea.h"
#include "../include/solutions.h"
#include "../include/flow.h"

// @param hash_len Always 128
char* hash (const char* input, const size_t input_len, const size_t salting_rounds) {
    #define LIMIT 128
    #define BLEN 32

    // Initialization
    uint8_t hash_box[LIMIT] = {0};
	uint8_t* input_bytes = (uint8_t*)input;

    // Input handling
    if (!input) {
        return NULL;
    }

    // Apply Solutions

    if (salting_rounds > 0) {
        size_t salted_len = 0;
        
        // Safe dynamic allocation of arrays 
        uint8_t* temp = malloc(SIZE_MAX * sizeof(uint8_t));
	    temp = salt(&input_bytes, input_len, salting_rounds, &salted_len);
        uint8_t* salted = realloc(temp, salted_len * sizeof(uint8_t));

        free(temp);

        // Compress or Fill
        cof(&salted, salted_len, &hash_box, LIMIT);
        free(salted);
    }

    else {
        // Compress or Fill
        cof(&input_bytes, input_len, &hash_box, LIMIT);
    }

    // Divide normalized input into 32 of 32 bits (4 bytes)

    uint32_t blocks[BLEN] = {0};
    disassemble_blocks(&hash_box, LIMIT, &blocks, BLEN);

    // Hashing

    apply(&blocks, BLEN);

    // Turning uint8_t array into character array.

    char *hash = malloc(LIMIT * 2 + 1);
    if (!hash) {
        return NULL;
    }

    static const char hex_digits[] = "0123456789abcdef";

    for (size_t i = 0; i < LIMIT; i++) {
        hash[i * 2]     = hex_digits[hash_box[i] >> 4];
        hash[i * 2 + 1] = hex_digits[hash_box[i] & 0x0F];
    }

    hash[LIMIT * 2] = '\0';

    // END, call free(hash) after secure depositation
    return hash;
}