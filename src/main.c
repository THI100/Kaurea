#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/kaurea.h"

int main () {
    #define H_LEN 257

    char test[] = "0";
    size_t size = sizeof(test) / sizeof(test[0]);
    size_t len = 0;

    printf("text: %s, size: %zu\n", test, size);

    char* hash_final = malloc(H_LEN * sizeof(char));
    hash_final = hash(test, size, 16, len);

    printf("%s\n", hash_final);

    free(hash_final);

    return 0;
}