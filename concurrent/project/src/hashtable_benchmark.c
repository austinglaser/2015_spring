/**
 * @file    hashtable_benchmark.c
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Benchmarking code for hashtable parallelism speedup
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module
#include "hashtable.h"

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <pthread.h>

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_ELEMENTS(a)   (sizeof(a)/sizeof((a)[0]))

#define N_KEYS              (10000)

/* --- PRIVATE VARIABLES ---------------------------------------------------- */

/**
 * @brief   The different numbers of threads to test with
 */
static const uint32_t n_threads[] = {1, 2, 4, 8, 16, 32, 64};

static volatile bool start_operation = false;

static uint32_t keys[N_KEYS];

static volatile atomic_uint_fast32_t key_index;

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Test hash function for an int
 */
static uint32_t hash_int(hashtable_key_t k);

/**
 * @brief   Dummy print
 */
static void print_elem(hashtable_elem_t e);

/**
 * @brief   Returns the us delta between two times
 *
 * @note    Adapted from http://stackoverflow.com/questions/10192903/time-in-milliseconds
 */
double timedifference_sec(struct timeval t0, struct timeval t1);

/**
 * @brief   Inserts into and then removes from a hashtable
 *
 * Waits on a signal to start running, then tries to insert many values
 * into a hashtable
 *
 * @param[in,out] arg:      A pointer to the hashtable to work on
 */
static void* test_thread_f(void* arg);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

int main(void)
{
    uint32_t i;

    // Generate key values
    for (i = 0; i < N_KEYS; i++) keys[i] = i;

    // Shuffle the numbers
    srand(time(NULL));
    for (i = 0; i < N_KEYS*4; i++) {
        uint32_t first_loc = rand() % N_KEYS;
        uint32_t second_loc = rand() % N_KEYS;
        uint32_t key_temp = keys[first_loc];
        keys[first_loc] = keys[second_loc];
        keys[second_loc] = key_temp;
    }

    // Loop over all different thread counts
    for (i = 0; i < ARRAY_ELEMENTS(n_threads); i++) {
        // Allocate thread array
        pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t)*n_threads[i]);
        if (!threads) {
            free(keys);
            printf("memory allocation failed\n");
            return 1;
        }

        // Create data structure
        hashtable_t h = hashtable_create(hash_int, print_elem, NULL);

        // Create threads
        start_operation = false;
        atomic_init(&key_index, 0);
        uint32_t thread_n;
        for (thread_n = 0; thread_n < n_threads[i]; i++) pthread_create(&(threads[thread_n]), NULL, test_thread_f, h);

        // Start threads and timer
        struct timeval start;
        gettimeofday(&start, NULL);
        start_operation = true;

        // Wait on threads
        for (thread_n = 0; thread_n < n_threads[i]; i++) pthread_join(threads[thread_n], NULL);

        // Stop timer
        struct timeval stop;
        gettimeofday(&stop, NULL);

        // Report results
        printf("%d threads\t%0.6lf seconds\n", n_threads[i], timedifference_sec(stop, start));

        // Free
        hashtable_free(h);
        free(threads);
    }

    // Free key array
    free(keys);
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static uint32_t hash_int(hashtable_key_t k)
{
    // Double cast to avoid compiler warning
    return (uint32_t)(uintptr_t) k;
}

static void print_elem(hashtable_elem_t e)
{
    (void) e;
}

static void* test_thread_f(void* arg)
{
    // Argument is really a hashtable type
    hashtable_t h = (hashtable_t) arg;

    // Wait for start signal
    while (!start_operation);

    // Insert everything
    uint_fast32_t current_index = atomic_fetch_add(&key_index, 1);
    while (current_index < N_KEYS) {
        bool success = hashtable_insert(h, (void*)(uintptr_t) keys[current_index], NULL);
        if (!success) printf("insertion failure\n");
    }

    // All done
    return NULL;
}

double timedifference_sec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) + ((t1.tv_usec - t0.tv_usec) / 1000000.0f);
}

