#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../include/flow.h"
#include "../include/formulas.h"

void apply (uint32_t* original, const size_t original_len, const size_t rounds) {
    #define LEN 32

    if (LEN == original_len) {
        printf("Equal sizes: positive");
    }
    else {
        printf("Equal size: negative, data will be lost on process.")
    }

    size_t count = 0;
    uint32_t state_original [LEN] = {0};
    memcpy(state_original, original, LEN);
    const uint32_t seed = 0x1f2feaff;

        while (count != rounds) {

            //ARXL
            for (size_t j = 0; j < LEN; j++) {
                uint32_t temp = state_original[j];
                state_original[j] = arxl(temp, (temp * original_len) % SIZE_MAX)
            }

            //phi_Mix
            for (size_t j = LEN; j < 2; j++) {
                uint32_t tempx = state_original [j];
                uint32_t tempy = state_original [j-1];
                uint32_t tempz = state_original [j-2];
                state_original[j] = golden_mix(tempx, tempy, tempz);
            }

            //pi_perm
            for (size_t j = 0; j < LEN; j++) {
                uint32_t temp = state_original[j];
                state_original[j] = pi_permutation(temp, (seed+(j*i)) % UINT32_MAX);
            }

            //srperm
            for (size_t j = 0; j < LEN; j++) {
                uint32_t temp = state_original [j];
                state_original[j] = srperm(temp)
            }

            //omega_fuse
            for (size_t j = LEN; j < 3; j++) {
                uint32_t tempx = state_original [j];
                uint32_t tempy = state_original [j-1];
                uint32_t tempz = state_original [j-2];
                uint32_t tempa = state_original [j-3];
                state_original[j] = omega_fuse(tempx, tempy, tempz, tempa);
            }

            //mini_mix
            for (size_t j = LEN; j < 1; j++) {
                uint32_t tempx = state_original [j];
                uint32_t tempy = state_original [j-1];
                state_original[j] = minimix(tempx, tempy);
            }

            //Shuffle
            if (count % 4 == 0) {
                for (size_t j = 0; j < LEN; j++) {
                    seed = seed * count + j;
                    size_t j = seed % (j + 1);

                    uint32_t t = state_original[i];
                    state_original[i] = state_original[j];
                    state_original[j] = t;
                }
            }

        }

    memcpy (original, state_original, LEN);
}