#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/kaurea.h"
#include "../include/solutions.h"

char* hash (const char* input, const size_t input_len, const size_t salting_rounds) {
    #define LIMIT 128

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

    return hash;
}