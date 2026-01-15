#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/kaurea.h"

int main () {
    #define MAXLEN 257

    char test[] = "Hello";
    size_t size = sizeof(test) / sizeof(test[0]);

    char* hashed = malloc(MAXLEN * sizeof(char));
    hashed = hash(&test, size, 8);
    printf("%s/n", hashed);
    free(hashed);

    return 0;
}