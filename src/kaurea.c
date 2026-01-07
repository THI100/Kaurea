#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/kaurea.h"
#include "../include/salting.h"

char* hash (const char* input, const size_t salting_rounds) {

    // Initialization

    const size_t LIMIT = 128;
    uint8_t hashBox[LIMIT];
    memset(hashBox, 0, LIMIT);
	uint8_t* input_bytes = (uint8_t*)input;

    // Input handling

    if (!input) {
        return NULL;
    }

    // Apply Salting
    
	salt(&input_bytes, salting_rounds);

    return hash;
}