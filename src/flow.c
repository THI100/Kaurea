#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../include/flow.h"
#include "../include/formulas.h"

void apply (uint32_t* original, const size_t original_len) {
    uint32_t* state_of_original [original_len] = {0};
    memcpy(state_of_original, original, original_len);
}