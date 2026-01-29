#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

#include "../include/kaurea.h"

void print_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Memory used: %ld kilobytes\n", usage.ru_maxrss);
}

int main () {
    // Starting variables
    #define H_LEN 257

    clock_t start, end;
    double cpu_time_used;
    char test[128] = "";
    scanf("%s", test);
    size_t size = sizeof(test) / sizeof(test[0]);
    size_t len = 0;

    printf("text: %s, size: %zu\n", test, size);

    // Starting hashing/process
    start = clock();

    char* hash_final = malloc(H_LEN * sizeof(char));
    hash_final = hash(test, size, 16, len);

    printf("%s\n", hash_final);

    print_memory_usage();

    // Ending process
    free(hash_final);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Execution time: %.6f seconds\n", cpu_time_used);

    return 0;
}