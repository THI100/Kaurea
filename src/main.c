#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/kaurea.h"

int main () {
    char test[] = "Hello";
    size_t size = sizeof(test) / sizeof(test[0]);
    size_t len = 0;

    printf("text: %s, size: %zu\n", test, size);

    char* temp = hash(&test, size, 16, &len);
    char* hash = realloc(temp, len * sizeof(char));

    if (hash == NULL) {
        free(temp);
        return 1;
    }

    printf("%s\n", hash);

    free(hash);

    return 0;
}