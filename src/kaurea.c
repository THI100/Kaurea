#include <stdint.h>
#include <stdlib.h>

#include "../include/kaurea.h"

char* hash (const char* input, uint64_t salting) {

        if (!input) {
        return NULL;
    }

    const size_t LIMIT = 128;
    uint8_t hashBox[LIMIT];
    memset(hashBox, 0, LIMIT);

    size_t inputSize = strlen(input);

    char* hash = "b1946ac92492d2347c6235b4d2611184";

    return hash;
}