/**
 * @file    reference_list_test.c
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Unit test for reference lists
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module under test
#include "reference_list.h"

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

// Modules
#include "unit_test.h"

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define N_STRESS_INSERTIONS     (4096)
#define N_THREADS               (2)

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Initializes context to a reference_list object
 */
static bool test_reference_list_standard_pre(void** p_context, char** err_str);

/**
 * @brief   Deallocates everything
 */
static void test_reference_list_standard_post(void* p_context);

/**
 * @brief   Tests creation and deletion of empty reference list
 */
static bool test_reference_list_create(void* p_context, char** err_str);

/**
 * @brief   Tests insertion into a list
 *
 * @note    The real test here is in the cleanup function, and valgrind
 *          telling us everything's hunky-dory
 */
static bool test_reference_list_insert(void* p_context, char** err_str);

/**
 * @brief   Stress test on the structure
 */
static bool test_reference_list_stress(void* p_context, char** err_str);

/**
 * @brief   Ensures the structure is thread-safe
 */
static bool test_reference_list_threading(void* p_context, char** err_str);

/**
 * @brief   Inserts a bunch of values into the reference list
 */
static void* test_reference_list_insert_thread_f(void* p_context);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

/**
 * @brief   Test entry point
 *
 * @return  1 if one or more tests failed, 0 otherwise
 */
int main(void)
{
    uint32_t err;
    unit_test_t reference_list_tests;

    // Allocate test structure
    reference_list_tests = unit_test_create("reference list");

    // Register tests
    unit_test_register(reference_list_tests,
                       "creation",
                       test_reference_list_standard_pre,
                       test_reference_list_create,
                       test_reference_list_standard_post);
    unit_test_register(reference_list_tests,
                       "insert",
                       test_reference_list_standard_pre,
                       test_reference_list_insert,
                       test_reference_list_standard_post);
    unit_test_register(reference_list_tests,
                       "stress",
                       test_reference_list_standard_pre,
                       test_reference_list_stress,
                       test_reference_list_standard_post);
    unit_test_register(reference_list_tests,
                       "threading",
                       test_reference_list_standard_pre,
                       test_reference_list_threading,
                       test_reference_list_standard_post);

    // Run tests
    if (unit_test_run(reference_list_tests)) err = 1;
    else                                     err = 0;

    // Free test structure
    unit_test_free(reference_list_tests);

    return err;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static bool test_reference_list_standard_pre(void** p_context, char** err_str)
{
    // Allcoate context
    reference_list_t r = reference_list_create(free);
    if (!r) {
        *err_str = "memory allocation failed";
        return false;
    }

    // Save allocated list
    *p_context = r;

    // Success
    *err_str = NULL;
    return true;
}

static void test_reference_list_standard_post(void* p_context)
{
    reference_list_t r = (reference_list_t) p_context;

    // Free the list, and all contained references
    reference_list_free(r);
}

static bool test_reference_list_create(void* p_context, char** err_str)
{
    (void) p_context;

    // The actual test happens in the pre-test; this is just a passthrough
    *err_str = NULL;
    return true;
}

static bool test_reference_list_insert(void* p_context, char** err_str)
{
    reference_list_t r = (reference_list_t) p_context;

    // Insert a reference
    uint32_t* reference = (uint32_t*) malloc(sizeof(uint32_t));
    uint32_t err_code = reference_list_insert(r, reference);
    if (err_code) {
        free(reference);
        *err_str = "insertion failed";
        return false;
    }

    // Successful insertion. Success criteria is also a clean
    // run with valgrind or another memory error-detection utility
    *err_str = NULL;
    return true;
}

static bool test_reference_list_stress(void* p_context, char** err_str)
{
    reference_list_t r = (reference_list_t) p_context;

    // Insert a lot of things
    uint32_t i;
    for (i = 0; i < N_STRESS_INSERTIONS; i++) {
        uint32_t* reference = (uint32_t*) malloc(sizeof(uint32_t));
        uint32_t err_code = reference_list_insert(r, reference);
        if (err_code) {
            free(reference);
            *err_str = "insertion failed";
            return false;
        }
    }

    // Successful insertion. Success criteria is also a clean
    // run with valgrind or another memory error-detection utility
    *err_str = NULL;
    return true;
}

static bool test_reference_list_threading(void* p_context, char** err_str)
{
    uint32_t i;

    // Start the insertion threads
    pthread_t insert_threads[N_THREADS];
    for (i = 0; i < N_THREADS; i++) {
        pthread_create(&(insert_threads[i]), NULL, test_reference_list_insert_thread_f, p_context);
    }

    // Wait on and check the insertion threads
    bool insert_success = true;
    for (i = 0; i < N_THREADS; i++) {
        void * err_val;
        pthread_join(insert_threads[i], &err_val);
        if (err_val) insert_success = false;
    }
    if (!insert_success) {
        *err_str = "insertion failed";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}

static void* test_reference_list_insert_thread_f(void* p_context)
{
    reference_list_t r = (reference_list_t) p_context;

    // Insert a lot of things
    uint32_t i;
    for (i = 0; i < N_STRESS_INSERTIONS; i++) {
        uint32_t* reference = (uint32_t*) malloc(sizeof(uint32_t));
        uint32_t err_code = reference_list_insert(r, reference);
        if (err_code) {
            free(reference);
            return (void*) 1;
        }
    }

    // Successful insertion. Success criteria is also a clean
    // run with valgrind or another memory error-detection utility
    return (void*) 0;
}
