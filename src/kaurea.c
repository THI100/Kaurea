#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/kaurea.h"
#include "../include/salting.h"

char* hash (const char* input, const size_t salting_rounds, const size_t input_len) {
    #define LIMIT 128

    // Initialization
    uint8_t hash_box[LIMIT] = {0};
	uint8_t* input_bytes = (uint8_t*)input;

    // Input handling
    if (!input) {
        return NULL;
    }

    // Apply Salting
    size_t salted_len = 0;

    // Missing allocation of the memory
	uint8_t* salted = salt(&input_bytes, input_len, salting_rounds, &salted_len);

    // Compress or Fill
    
    // cof(&salted, salted_len, &hash_box, LIMIT);

    free(salted);
    

    return hash;
}