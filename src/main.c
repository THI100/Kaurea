#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#include "../include/kaurea.h"

#define INPUT_MAX 256
#define H_LEN 257

void print_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Memory used: %ld kilobytes\n", usage.ru_maxrss);
}

int main(void) {
    clock_t start, end;
    double cpu_time_used;

    char test[INPUT_MAX];

    printf("Write your input: ");
    if (scanf("%255s", test) != 1) {
        printf("Not possible to read input.\n");
        return 1;
    }

    size_t size = strlen(test);
    printf("text: %s, size: %zu\n", test, size);

    start = clock();

    char *hash_final = hash(test, size, 16, H_LEN);
    if (!hash_final) {
        printf("Hash failed.\n");
        return 1;
    }

    printf("%s\n", hash_final);
    print_memory_usage();

    free(hash_final);

    end = clock();
    cpu_time_used = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %.6f seconds\n", cpu_time_used);

    return 0;
}