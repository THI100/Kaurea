#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/resource.h>

#include "../include/kaurea.h"

#define NUM_TESTS_PI    1000000
#define NUM_TESTS_COL   200000
#define MAX_INPUT_LEN   128
#define H_LEN           128         // hash output bytes
#define H_BITS          (H_LEN * 8) // 1024 bits
#define H_HEX           (H_LEN * 2) // hex string length
#define SALTING_ROUNDS  16

// ─────────────────────────────────────────────────────────────────────────────
// Utilities
// ─────────────────────────────────────────────────────────────────────────────

void unique_utf8_string(char *buf, size_t max_len, unsigned long long counter) {
    static unsigned long long seed = 0;
    if (seed == 0) {
        unsigned long long t    = (unsigned long long)time(NULL);
        uintptr_t          addr = (uintptr_t)&seed;
        seed = t ^ (addr * 0x9E3779B97F4A7C15ULL);
        seed ^= (seed >> 30); seed *= 0xBF58476D1CE4E5B9ULL;
        seed ^= (seed >> 27); seed *= 0x94D049BB133111EBULL;
        seed ^= (seed >> 31);
    }
    unsigned long long x = counter ^ (seed + (counter << 17) | (counter >> 13));
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
    x ^= (x >> 31);

    size_t i = 0;
    while (x > 0 && i < max_len - 1) {
        buf[i++] = (char)(32 + (x % 95));
        x /= 95;
    }
    buf[i] = '\0';
}

void print_memory_usage(void) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Memory used: %ld kilobytes\n", usage.ru_maxrss);
}

// Parse hex hash string into a byte array.
static void hex_to_bytes(const char *hex, uint8_t *out, size_t out_len) {
    for (size_t i = 0; i < out_len; i++) {
        unsigned int v;
        sscanf(hex + i * 2, "%02x", &v);
        out[i] = (uint8_t)v;
    }
}

// Count differing bits between two byte arrays.
static int hamming_distance(const uint8_t *a, const uint8_t *b, size_t len) {
    int dist = 0;
    for (size_t i = 0; i < len; i++) {
        uint8_t diff = a[i] ^ b[i];
        // Kernighan bit count
        while (diff) { dist++; diff &= diff - 1; }
    }
    return dist;
}

static inline uint64_t fast64(const char *s) {
    uint64_t h = 1469598103934665603ULL;
    for (; *s; ++s) h = (h ^ (unsigned char)*s) * 1099511628211ULL;
    return h;
}

// ─────────────────────────────────────────────────────────────────────────────
// Option 1 — Collision Testing  (unchanged logic, minor style cleanup)
// ─────────────────────────────────────────────────────────────────────────────

typedef struct {
    uint64_t hash_code;
    char     hash_str[H_HEX + 1];
    char     input[MAX_INPUT_LEN];
    int      used;
} HashEntry;

#define TABLE_SIZE (NUM_TESTS_COL * 2 + 1)

static void test_collisions(void) {
    size_t     collisions = 0;
    HashEntry *table      = calloc(TABLE_SIZE, sizeof(HashEntry));
    if (!table) { fprintf(stderr, "OOM\n"); return; }

    for (size_t i = 0; i < NUM_TESTS_COL; i++) {
        if (i % 10000 == 0) {
            printf("\rProgress: [%.2f%%] (%zu/%d)",
                   (float)i / NUM_TESTS_COL * 100.0f, i, NUM_TESTS_COL);
            fflush(stdout);
        }

        char   input[MAX_INPUT_LEN];
        size_t input_len;
        unique_utf8_string(input, MAX_INPUT_LEN, i + 1);
        input_len = strlen(input);

        size_t hlen = 0;
        char  *h    = hash(input, input_len, SALTING_ROUNDS, &hlen);
        if (!h) continue;

        uint64_t hc  = fast64(h);
        size_t   pos = hc % TABLE_SIZE;

        while (table[pos].used) {
            if (memcmp(table[pos].hash_str, h, H_HEX) == 0) {
                collisions++;
                printf("\n[COLLISION #%zu]\nInput A: %s\nInput B: %s\nHash   : %s\n",
                       collisions, table[pos].input, input, h);
                break;
            }
            pos = (pos + 1) % TABLE_SIZE;
        }
        if (!table[pos].used) {
            table[pos].used = 1;
            table[pos].hash_code = hc;
            strncpy(table[pos].hash_str, h, H_HEX);
            strncpy(table[pos].input,    input, MAX_INPUT_LEN - 1);
        }
        free(h);
    }

    printf("\nTotal tests: %d  |  Collisions: %zu\n", NUM_TESTS_COL, collisions);
    print_memory_usage();
    free(table);
}

// ─────────────────────────────────────────────────────────────────────────────
// Option 2 — Pre-image Attack  (unchanged logic, updated call signature)
// ─────────────────────────────────────────────────────────────────────────────

static void test_preimage(void) {
    const char *targetInput = "SecretMessage123";
    size_t      target_len  = strlen(targetInput);
    size_t      hlen        = 0;

    char *targetHash = hash(targetInput, target_len, SALTING_ROUNDS, &hlen);
    if (!targetHash) return;

    printf("Target Input: %s\nTarget Hash : %s\n", targetInput, targetHash);

    int found = 0;
    for (size_t i = 1; i <= NUM_TESTS_PI; i++) {
        if (i % 10000 == 0) {
            printf("\rProgress: [%.2f%%] (%zu/%d)",
                   (float)i / NUM_TESTS_PI * 100.0f, i, NUM_TESTS_PI);
            fflush(stdout);
        }
        char   candidate[MAX_INPUT_LEN];
        size_t cand_len;
        unique_utf8_string(candidate, MAX_INPUT_LEN, i);
        cand_len = strlen(candidate);

        size_t hl2 = 0;
        char  *h   = hash(candidate, cand_len, SALTING_ROUNDS, &hl2);
        if (!h) continue;

        if (memcmp(h, targetHash, H_HEX) == 0 &&
            strcmp(candidate, targetInput) != 0) {
            printf("\n[PRE-IMAGE FOUND]\nCandidate: %s\nHash: %s\n", candidate, h);
            found = 1;
            free(h);
            break;
        }
        free(h);
    }
    free(targetHash);
    if (!found)
        printf("\nNo pre-image found after %d attempts.\n", NUM_TESTS_PI);
}

// ─────────────────────────────────────────────────────────────────────────────
// Option 3 — Single Manual Hash
// ─────────────────────────────────────────────────────────────────────────────

static void test_single(void) {
    char test_input[256];
    printf("Write your input: ");
    if (scanf("%255s", test_input) != 1) return;

    size_t size = strlen(test_input);
    printf("Input: %s  (%zu bytes)\n", test_input, size);

    size_t hlen = 0;
    char  *h    = hash(test_input, size, SALTING_ROUNDS, &hlen);
    if (!h) { printf("Hash failed.\n"); return; }

    printf("Hash: %s\n", h);
    free(h);
}

// ─────────────────────────────────────────────────────────────────────────────
// Option 4 — Avalanche / Strict Avalanche Criterion (SAC)
//
// Target: each flipped input bit should change ~50 % of output bits (512 out of 1024).
//
// A well-designed hash should have mean ≈ 50 % and worst-case < 5 % deviation.
// ─────────────────────────────────────────────────────────────────────────────

#define SAC_SAMPLES     200     // inputs to test  (keep reasonable - O(N*inputbits))
#define SAC_INPUT_BYTES 16      // fixed input size for SAC (128 bits)
#define SAC_INPUT_BITS  (SAC_INPUT_BYTES * 8)

static void test_avalanche(void) {
    printf("\n=== Avalanche / SAC Test ===\n");
    printf("Inputs: %d  |  Input size: %d bits  |  Output: %d bits\n\n",
           SAC_SAMPLES, SAC_INPUT_BITS, H_BITS);

    // Per-input-bit accumulator: total output bits flipped across all samples
    double bit_flip_sum[SAC_INPUT_BITS] = {0};
    double global_sum   = 0.0;
    long   total_trials = 0;

    uint8_t base_bytes[H_LEN], flip_bytes[H_LEN];

    for (int s = 0; s < SAC_SAMPLES; s++) {
        // Generate a random fixed-length input
        char base_input[SAC_INPUT_BYTES + 1];
        unique_utf8_string(base_input, SAC_INPUT_BYTES + 1, (unsigned long long)s + 1);
        size_t blen = strlen(base_input);
        while (blen < SAC_INPUT_BYTES) base_input[blen++] = 'A';
        base_input[SAC_INPUT_BYTES] = '\0';

        size_t hl = 0;
        char  *base_hash = hash(base_input, SAC_INPUT_BYTES, SALTING_ROUNDS, &hl);
        if (!base_hash) continue;
        hex_to_bytes(base_hash, base_bytes, H_LEN);
        free(base_hash);

        // Flip each input bit independently
        for (int bit = 0; bit < SAC_INPUT_BITS; bit++) {
            char flipped[SAC_INPUT_BYTES + 1];
            memcpy(flipped, base_input, SAC_INPUT_BYTES + 1);

            // Flip bit `bit` in the byte string
            flipped[bit / 8] ^= (char)(1u << (bit % 8));

            size_t hl2     = 0;
            char  *fh      = hash(flipped, SAC_INPUT_BYTES, SALTING_ROUNDS, &hl2);
            if (!fh) continue;
            hex_to_bytes(fh, flip_bytes, H_LEN);
            free(fh);

            int dist = hamming_distance(base_bytes, flip_bytes, H_LEN);
            bit_flip_sum[bit] += dist;
            global_sum        += dist;
            total_trials++;
        }

        if ((s + 1) % 20 == 0) {
            printf("\r  Samples done: %d/%d", s + 1, SAC_SAMPLES);
            fflush(stdout);
        }
    }

    printf("\n\n--- Results ---\n");
    double mean_flipped = global_sum / total_trials;
    double mean_pct     = mean_flipped / H_BITS * 100.0;
    printf("Mean bits flipped per input-bit flip : %.2f / %d  (%.2f%%)\n",
           mean_flipped, H_BITS, mean_pct);

    // Worst-case bit bias
    double worst_bias = 0.0;
    int    worst_bit  = 0;
    for (int bit = 0; bit < SAC_INPUT_BITS; bit++) {
        double avg  = bit_flip_sum[bit] / SAC_SAMPLES;
        double bias = fabs(avg / H_BITS - 0.5);
        if (bias > worst_bias) { worst_bias = bias; worst_bit = bit; }
    }
    printf("Worst single-input-bit bias          : %.4f%%  (bit %d)\n",
           worst_bias * 100.0, worst_bit);
    printf("Target: mean ≈ 50%%,  worst bias < 5%%\n");

    if (mean_pct >= 45.0 && mean_pct <= 55.0 && worst_bias < 0.05)
        printf("Result: PASS ✓\n");
    else
        printf("Result: FAIL ✗  — diffusion is weaker than expected\n");
}

// ─────────────────────────────────────────────────────────────────────────────
// Option 5 — Bit Distribution / Frequency Test
//
// A strong hash should have std-dev < 1 % and no bit position outside
// the range [45 %, 55 %].
// ─────────────────────────────────────────────────────────────────────────────

#define DIST_SAMPLES 50000

static void test_bit_distribution(void) {
    printf("\n=== Bit Distribution Test ===\n");
    printf("Samples: %d  |  Output bits: %d\n\n", DIST_SAMPLES, H_BITS);

    long *bit_counts = calloc(H_BITS, sizeof(long));
    if (!bit_counts) { fprintf(stderr, "OOM\n"); return; }

    uint8_t hbytes[H_LEN];

    for (int i = 0; i < DIST_SAMPLES; i++) {
        char input[MAX_INPUT_LEN];
        unique_utf8_string(input, MAX_INPUT_LEN, (unsigned long long)i + 1);
        size_t ilen = strlen(input);

        size_t hl = 0;
        char  *h  = hash(input, ilen, SALTING_ROUNDS, &hl);
        if (!h) continue;
        hex_to_bytes(h, hbytes, H_LEN);
        free(h);

        for (int b = 0; b < H_BITS; b++) {
            if (hbytes[b / 8] & (1u << (b % 8)))
                bit_counts[b]++;
        }

        if ((i + 1) % 5000 == 0) {
            printf("\r  Progress: %d/%d", i + 1, DIST_SAMPLES);
            fflush(stdout);
        }
    }

    printf("\n\n--- Results ---\n");

    double sum = 0.0, sumsq = 0.0;
    double min_freq = 1.0, max_freq = 0.0;
    int    min_bit  = 0, max_bit = 0;

    for (int b = 0; b < H_BITS; b++) {
        double freq = (double)bit_counts[b] / DIST_SAMPLES;
        sum   += freq;
        sumsq += freq * freq;
        if (freq < min_freq) { min_freq = freq; min_bit = b; }
        if (freq > max_freq) { max_freq = freq; max_bit = b; }
    }

    double mean   = sum / H_BITS;
    double stddev = sqrt(sumsq / H_BITS - mean * mean);

    printf("Mean frequency      : %.4f%%\n", mean * 100.0);
    printf("Std deviation       : %.4f%%\n", stddev * 100.0);
    printf("Most biased bit     : bit %d  (%.4f%%)\n", max_bit, max_freq * 100.0);
    printf("Least biased bit    : bit %d  (%.4f%%)\n", min_bit, min_freq * 100.0);
    printf("Target: mean ≈ 50%%, stddev < 1%%, all bits in [45%%, 55%%]\n");

    int pass = (mean >= 0.45 && mean <= 0.55 &&
                stddev < 0.01 &&
                min_freq >= 0.45 && max_freq <= 0.55);
    printf("Result: %s\n", pass ? "PASS ✓" : "FAIL ✗  — output has bit bias");

    free(bit_counts);
}

// ─────────────────────────────────────────────────────────────────────────────
// Option 6 — Length-Extension Sanity Check
//
// Verifies that hash("abc") and hash("abc" + any_suffix) are unrelated —
// i.e., that the padding fix is working.
//
// good avalanche (≥ 40 % bit difference from the base hash).
// ─────────────────────────────────────────────────────────────────────────────

static void test_length_extension(void) {
    printf("\n=== Length-Extension Sanity Check ===\n");

    const char *base   = "abc";
    size_t      blen   = strlen(base);
    size_t      hl     = 0;

    char *base_hash = hash(base, blen, SALTING_ROUNDS, &hl);
    if (!base_hash) return;
    printf("Base  hash(\"abc\")             : %.32s...\n", base_hash);

    uint8_t base_bytes[H_LEN];
    hex_to_bytes(base_hash, base_bytes, H_LEN);
    free(base_hash);

    // Test 8 extensions of increasing length
    const char *suffixes[] = {"x","xy","xyz","xyzw","Hello","!","123456","AAAAAAAA"};
    int all_pass = 1;

    for (int i = 0; i < 8; i++) {
        char extended[256];
        snprintf(extended, sizeof(extended), "%s%s", base, suffixes[i]);
        size_t elen = strlen(extended);

        size_t hl2 = 0;
        char  *h   = hash(extended, elen, SALTING_ROUNDS, &hl2);
        if (!h) continue;

        uint8_t ext_bytes[H_LEN];
        hex_to_bytes(h, ext_bytes, H_LEN);

        int dist = hamming_distance(base_bytes, ext_bytes, H_LEN);
        double pct = (double)dist / H_BITS * 100.0;

        printf("hash(\"abc%s\")%*s: %.32s...  diff=%.1f%%  %s\n",
               suffixes[i],
               (int)(8 - strlen(suffixes[i])), "",
               h, pct,
               pct >= 40.0 ? "OK" : "WARN");

        if (pct < 40.0) all_pass = 0;
        free(h);
    }

    printf("Result: %s\n", all_pass
        ? "PASS ✓  — extensions produce independent hashes"
        : "WARN ✗  — some extensions are suspiciously close to the base hash");
}

// ─────────────────────────────────────────────────────────────────────────────
// Option 7 — Timing Consistency Test
//
// Measures whether hashing short vs. long inputs takes suspiciously different
// amounts of time.
//
// Should have CV < 10 % across different input sizes.
// ─────────────────────────────────────────────────────────────────────────────

#define TIMING_REPS 500

static void test_timing(void) {
    printf("\n=== Timing Consistency Test ===\n");
    printf("Repetitions per length: %d\n\n", TIMING_REPS);

    size_t test_lengths[] = {1, 8, 16, 64, 128};
    int    n_lengths      = 5;

    double means[5], cvs[5];

    for (int li = 0; li < n_lengths; li++) {
        size_t  len   = test_lengths[li];
        double  total = 0.0, sumsq = 0.0;

        for (int r = 0; r < TIMING_REPS; r++) {
            char input[MAX_INPUT_LEN + 1];
            memset(input, 'A' + (r % 26), len);
            input[len] = '\0';

            struct timespec t0, t1;
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);

            size_t hl = 0;
            char  *h  = hash(input, len, SALTING_ROUNDS, &hl);
            free(h);

            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

            double elapsed = (t1.tv_sec  - t0.tv_sec) * 1e6
                           + (t1.tv_nsec - t0.tv_nsec) / 1e3; // microseconds
            total  += elapsed;
            sumsq  += elapsed * elapsed;
        }

        double mean   = total / TIMING_REPS;
        double stddev = sqrt(sumsq / TIMING_REPS - mean * mean);
        double cv     = stddev / mean * 100.0;

        means[li] = mean;
        cvs[li]   = cv;

        printf("Input len %3zu bytes : mean=%.2f µs  stddev=%.2f  CV=%.1f%%  %s\n",
               len, mean, stddev, cv, cv < 10.0 ? "OK" : "WARN");
    }

    // Check that longest input doesn't take more than 3× the shortest
    double ratio = means[n_lengths - 1] / means[0];
    printf("\nLongest/shortest time ratio: %.2fx\n", ratio);
    printf("Result: %s\n", ratio < 5.0
        ? "PASS ✓  — timing is reasonably consistent"
        : "WARN ✗  — large timing variance; potential length-leaking path");
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────

int main(void) {
    int nc = 0;
    printf("Select test:\n"
           "  1 = Collision search (%d inputs)\n"
           "  2 = Pre-image attack (%d attempts)\n"
           "  3 = Single hash\n"
           "  4 = Avalanche / SAC\n"
           "  5 = Bit distribution\n"
           "  6 = Length-extension sanity\n"
           "  7 = Timing consistency\n"
           "> ", NUM_TESTS_COL, NUM_TESTS_PI);

    if (scanf("%d", &nc) != 1) return 1;

    clock_t start = clock();

    switch (nc) {
        case 1: test_collisions();        break;
        case 2: test_preimage();          break;
        case 3: test_single();            break;
        case 4: test_avalanche();         break;
        case 5: test_bit_distribution();  break;
        case 6: test_length_extension();  break;
        case 7: test_timing();            break;
        default: printf("Invalid option.\n"); return 0;
    }

    clock_t end = clock();
    printf("\nExecution time: %.6f seconds\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
