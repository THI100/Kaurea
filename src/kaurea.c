#include <stdint.h>
#include <stdlib.h>

#include "../include/kaurea.h"
#include "../include/salting.h"

char* hash (const char* input, const uint16_t salting) {

    // Initialization

    const size_t LIMIT = 128;
    uint8_t hashBox[LIMIT];
    memset(hashBox, 0, LIMIT);

    // Input handling

    if (!input) {
        return NULL;
    }

    // Apply Salting
    


    return hash;
}