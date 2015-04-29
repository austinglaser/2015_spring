/**
 * @file    hashtable_test.c
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Unit test for concurrent hashtable implementation
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module under test
#include "hashtable.h"

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// Modules
#include "unit_test.h"

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_ELEMENTS(a)   (sizeof(a)/sizeof((a)[0]))

#define N_STRESS_INSERTIONS (5200)          // Not a power of two

//#define VERBOSE

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Data structure for a standard hashtable test
 */
typedef struct hashtable_test_context_t_ {
    hashtable_t int_table;          /**< A table of ints */
    hashtable_t string_table;       /**< A table of strings */
} * hashtable_test_context_t;

/**
 * @brief   Data structure for a stress test
 */
typedef struct hashtable_stress_context_t_ {
    hashtable_t int_table;          /**< A table of ints */
    uint32_t * keys;                /**< Some keys to insert */
    char ** elems;                  /**< Some elements to insert with them */
} * hashtable_stress_context_t;

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Test hash function for an int
 */
static uint32_t hash_int(hashtable_key_t k);

/**
 * @brief   Test hash function for a string
 *
 * @note    Implementation from http://stackoverflow.com/questions/7666509/hash-function-for-string, http://www.cse.yorku.ca/~oz/hash.html
 */
static uint32_t hash_string(hashtable_key_t k);

/**
 * @brief   Prints a single table element
 */
static void print_elem(hashtable_elem_t e);

/**
 * @brief   Initiializes context to the standard struct
 */
static bool test_hashtable_standard_pre(void ** p_context, char ** err_str);

/**
 * @brief   Deallocates everything in the standard case
 */
static void test_hashtable_standard_post(void * p_context);

/**
 * @brief   Initiializes context to the stress struct
 */
static bool test_hashtable_stress_pre(void ** p_context, char ** err_str);

/**
 * @brief   Deallocates everything in the stress struct
 */
static void test_hashtable_stress_post(void * p_context);

/**
 * @brief   Tests creation of a hashtable
 */
static bool test_hashtable_create(void * p_context, char ** err_str);

/**
 * @brief   Tests insertion into a hashtable
 */
static bool test_hashtable_insert_contains(void * p_context, char ** err_str);

/**
 * @brief   Tests edge cases with insertion
 */
static bool test_hashtable_insert_edge_cases(void * p_context, char ** err_str);

/**
 * @brief   Tests contains with an element not present
 */
static bool test_hashtable_contains_not_present(void * p_context, char ** err_str);

/**
 * @brief   Tests that multiple insertions are disallowed
 */
static bool test_hashtable_duplicate_insertion(void * p_context, char ** err_str);

/**
 * @brief   Stress Test
 */
static bool test_hashtable_stress(void * p_context, char ** err_str);

/**
 * @brief   Tests getting an object back from the hashtable
 */
static bool test_hashtable_get(void * p_context, char ** err_str);

/**
 * @brief   Tests removing an object from the hashtable
 */
static bool test_hashtable_remove(void * p_context, char ** err_str);

/**
 * @brief   Test with threading
 *
 * @note    Unimplemented; will always fail. Probably will be several tests eventually
 */
static bool test_hashtable_threading(void * p_context, char ** err_str);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

/**
 * @brief   Test entry point
 *
 * @return  -1 if one or more tests failed, 0 otherwise
 */
int main(void)
{
    uint32_t err;
    unit_test_t hashtable_tests;

    // Allocate test structure
    hashtable_tests = unit_test_create("hashtable");

    // Register tests
    unit_test_register(hashtable_tests,
                       "creation",
                       test_hashtable_standard_pre,
                       test_hashtable_create,
                       test_hashtable_standard_post);
    unit_test_register(hashtable_tests,
                       "insertion and membership",
                       test_hashtable_standard_pre,
                       test_hashtable_insert_contains,
                       test_hashtable_standard_post);
    unit_test_register(hashtable_tests,
                       "insertion edge cases",
                       test_hashtable_standard_pre,
                       test_hashtable_insert_edge_cases,
                       test_hashtable_standard_post);
    unit_test_register(hashtable_tests,
                       "contains on non-present member",
                       test_hashtable_standard_pre,
                       test_hashtable_contains_not_present,
                       test_hashtable_standard_post);
    unit_test_register(hashtable_tests,
                       "duplicate insertion",
                       test_hashtable_standard_pre,
                       test_hashtable_duplicate_insertion,
                       test_hashtable_standard_post);
    unit_test_register(hashtable_tests,
                       "getting",
                       test_hashtable_standard_pre,
                       test_hashtable_get,
                       test_hashtable_standard_post);
    unit_test_register(hashtable_tests,
                       "removing",
                       test_hashtable_standard_pre,
                       test_hashtable_remove,
                       test_hashtable_standard_post);
    unit_test_register(hashtable_tests,
                       "stress",
                       test_hashtable_stress_pre,
                       test_hashtable_stress,
                       test_hashtable_stress_post);
    unit_test_register(hashtable_tests,
                       "threading",
                       test_hashtable_standard_pre,
                       test_hashtable_threading,
                       test_hashtable_standard_post);

    // Run tests
    if (unit_test_run(hashtable_tests)) err = 1;
    else                                err = 0;

    // Free test structure
    unit_test_free(hashtable_tests);

    return err;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static uint32_t hash_int(hashtable_key_t k)
{
    // Double cast to avoid compiler warning
    return (uint32_t)(uintptr_t) k;
}

static uint32_t hash_string(hashtable_key_t k)
{
    uint32_t hash = 5381;
    uint32_t i;
    char * str = (char *) k;

    for (i = 0; str[i]; i++) hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */

    return hash;
}

static void print_elem(hashtable_elem_t e)
{
    // It's actually a string
    printf("\"%s\"", (char *) e);
}

static bool test_hashtable_standard_pre(void ** p_context, char ** err_str)
{
    hashtable_test_context_t context;

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!p_context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // Initialize error string
    *err_str = NULL;

    // Allocate context
    context = (hashtable_test_context_t) malloc(sizeof(struct hashtable_test_context_t_));
    if (!context) {
        *err_str = "test allocation failed";
        return false;
    }

    // Allocate tables
    context->int_table = hashtable_create(hash_int, print_elem, NULL);
    if (!context->int_table) {
        *err_str = "memory allocation failed";
        free(context);
        return false;
    }
    context->string_table = hashtable_create(hash_string, print_elem, NULL);
    if (!context->int_table) {
        *err_str = "memory allocation failed";
        hashtable_free(context->int_table);
        free(context);
        return false;
    }

    #ifdef VERBOSE
    printf("Initial empty int table:\n");
    hashtable_print(context->int_table);
    printf("\n");

    printf("Initial empty string table:\n");
    hashtable_print(context->string_table);
    printf("\n");
    #endif

    // Pass context on
    *p_context = context;

    // Successful allocation
    *err_str = NULL;
    return true;
}

static void test_hashtable_standard_post(void * p_context)
{
    hashtable_test_context_t context = (hashtable_test_context_t) p_context;

    // Free all the memory
    if (context) {
        if (context->int_table)     hashtable_free(context->int_table);
        if (context->string_table)  hashtable_free(context->string_table);

        free(context);
    }
}

static bool test_hashtable_stress_pre(void ** p_context, char ** err_str)
{
    hashtable_stress_context_t context;
    uint32_t i, j;

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!p_context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // Initialize error string
    *err_str = NULL;

    // Allocate context
    context = (hashtable_stress_context_t) malloc(sizeof(struct hashtable_stress_context_t_));
    if (!context) {
        *err_str = "test allocation failed";
        return false;
    }

    // Allocate table
    context->int_table = hashtable_create(hash_int, print_elem, NULL);
    if (!context->int_table) {
        *err_str = "memory allocation failed";
        free(context);
        return false;
    }

    #ifdef VERBOSE
    printf("Initial empty int table:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    // Allocate keys
    context->keys = (uint32_t *) malloc(sizeof(uint32_t) * N_STRESS_INSERTIONS);
    if (!context->keys) {
        *err_str = "memory allocation failed";
        free(context->int_table);
        free(context);
        return false;
    }

    // Fill with ascending numbers
    for (i = 0; i < N_STRESS_INSERTIONS; i++) context->keys[i] = i;

    // Shuffle the numbers
    srand(time(NULL));
    for (i = 0; i < N_STRESS_INSERTIONS*4; i++) {
        uint32_t first_loc = rand() % N_STRESS_INSERTIONS;
        uint32_t second_loc = rand() % N_STRESS_INSERTIONS;
        uint32_t key_temp = context->keys[first_loc];
        context->keys[first_loc] = context->keys[second_loc];
        context->keys[second_loc] = key_temp;
    }

    // Allocate element array
    context->elems = (char **) malloc(sizeof(char *) * N_STRESS_INSERTIONS);
    if (!context->elems) {
        *err_str = "memory allocation failed";
        free(context->keys);
        free(context->int_table);
        free(context);
        return false;
    }

    // Allocate elements
    for (i = 0; i < N_STRESS_INSERTIONS; i++) {
        context->elems[i] = (char *) malloc(sizeof(char) * 10);
        if (!context->elems[i]) {
            *err_str = "memory allocation failed";
            for (j = 0; j < i; j++) free(context->elems[j]);
            free(context->elems);
            free(context->keys);
            free(context->int_table);
            free(context);
            return false;
        }
    }

    // Fill elements
    for (i = 0; i < N_STRESS_INSERTIONS; i++) sprintf(context->elems[i], "%d", context->keys[i]);

    // Pass context on
    *p_context = context;

    // Successful allocation
    *err_str = NULL;
    return true;
}

static void test_hashtable_stress_post(void * p_context)
{
    hashtable_stress_context_t context = (hashtable_stress_context_t) p_context;

    // Free all the memory
    if (context) {
        if (context->elems) {
            uint32_t i;
            for (i = 0; i < N_STRESS_INSERTIONS; i++) {
                if (context->elems[i]) free(context->elems[i]);
            }
            free(context->elems);
        }
        if (context->keys)          free(context->keys);
        if (context->int_table)     hashtable_free(context->int_table);

        free(context);
    }
}

static bool test_hashtable_create(void * p_context, char ** err_str)
{
    (void) p_context;

    // Success; actual test happens in preamble
    *err_str = NULL;
    return true;
}

static bool test_hashtable_insert_contains(void * p_context, char ** err_str)
{
    hashtable_test_context_t context = (hashtable_test_context_t) p_context;
    bool success;
    bool present;

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // Initialize error string
    *err_str = NULL;

    // int insertion
    success = hashtable_insert(context->int_table, (void *) 5, "5 elem");
    if (!success) {
        *err_str = "int insertion failed";
        return false;
    }
    #ifdef VERBOSE
    printf("Inserted 5:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif


    // string insertion
    success = hashtable_insert(context->string_table, "five", "5 elem");
    if (!success) {
        *err_str = "string insertion failed";
        return false;
    }
    #ifdef VERBOSE
    printf("Inserted \"five\":\n");
    hashtable_print(context->string_table);
    printf("\n");
    #endif

    // int membership
    present = hashtable_contains(context->int_table, (void *) 5);
    if (!present) {
        *err_str = "int contains failed";
        return false;
    }

    // string membership
    present = hashtable_contains(context->string_table, "five");
    if (!present) {
        *err_str = "int contains failed";
        return false;
    }

    // Success
    return true;
}

static bool test_hashtable_insert_edge_cases(void * p_context, char ** err_str)
{
    hashtable_test_context_t context = (hashtable_test_context_t) p_context;
    bool success;
    bool present;

    // int insertion
    success = hashtable_insert(context->int_table, (void *) 4, "4 elem");
    if (!success) {
        *err_str = "int insertion failed";
        return false;
    }
    #ifdef VERBOSE
    printf("Inserted 4:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    // string insertion
    success = hashtable_insert(context->string_table, "four", "4 elem");
    if (!success) {
        *err_str = "string insertion failed";
        return false;
    }
    #ifdef VERBOSE
    printf("Inserted \"four\":\n");
    hashtable_print(context->string_table);
    printf("\n");
    #endif

    // int membership
    present = hashtable_contains(context->int_table, (void *) 4);
    if (!present) {
        *err_str = "int contains failed";
        return false;
    }

    // string membership
    present = hashtable_contains(context->string_table, "four");
    if (!present) {
        *err_str = "int contains failed";
        return false;
    }

    // int insertion
    success = hashtable_insert(context->int_table, (void *) 0, "0 elem");
    if (!success) {
        *err_str = "zero hash insertion failed";
        return false;
    }
    #ifdef VERBOSE
    printf("Inserted 0:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    present = hashtable_contains(context->int_table, (void *) 0);
    if (!present) {
        *err_str = "zero hash insertion failed";
        return false;
    }

    // int insertion
    success = hashtable_insert(context->int_table, (void *) UINT32_MAX, "effs elem");
    if (!success) {
        *err_str = "max hash insertion failed";
        return false;
    }
    #ifdef VERBOSE
    printf("Inserted 0xFFFFFFFF:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    present = hashtable_contains(context->int_table, (void *) UINT32_MAX);
    if (!present) {
        *err_str = "max hash insertion failed";
        return false;
    }

    // Passed all tests!
    return true;
}

static bool test_hashtable_contains_not_present(void * p_context, char ** err_str)
{
    hashtable_test_context_t context = (hashtable_test_context_t) p_context;
    bool present;
    bool success;
    uint32_t i;
    size_t int_keys[] = {1, 2, 3, -4, 6, 10};
    char * string_keys[] = {"one", "two", "three", "negative four", "six", "ten"};
    char * elems[] = {"1 elem", "2 elem", "3 elem", "-4 elem", "6 elem", "10 elem"};

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // Initialize error string
    *err_str = NULL;

    // Membership on empty int table
    present = hashtable_contains(context->int_table, (void *) 5);
    if (present) {
        *err_str = "contains on empty int table failed";
        return false;
    }

    // Membership on empty string table
    present = hashtable_contains(context->string_table, "five");
    if (present) {
        *err_str = "contains on empty string table failed";
        return false;
    }

    // Ensure arrays are the same size
    assert(ARRAY_ELEMENTS(int_keys)     == ARRAY_ELEMENTS(elems));
    assert(ARRAY_ELEMENTS(string_keys)  == ARRAY_ELEMENTS(elems));

    // Add some members to int table
    for (i = 0; i < ARRAY_ELEMENTS(int_keys); i++) {
        success = hashtable_insert(context->int_table, (void *) int_keys[i], elems[i]);
        if (!success) {
            *err_str = "insertion failed";
            return false;
        }
        #ifdef VERBOSE
        printf("int insertion %d:\n", i);
        hashtable_print(context->int_table);
        printf("\n");
        #endif
    }

    // Add some members to string table
    for (i = 0; i < ARRAY_ELEMENTS(string_keys); i++) {
        success = hashtable_insert(context->string_table, string_keys[i], elems[i]);
        if (!success) {
            *err_str = "insertion failed";
            return false;
        }
        #ifdef VERBOSE
        printf("string insertion %d:\n", i);
        hashtable_print(context->string_table);
        printf("\n");
        #endif
    }

    // Membership on non-empty int table
    present = hashtable_contains(context->int_table, (void *) 5);
    if (present) {
        *err_str = "contains on non-empty int table failed";
        return false;
    }

    // Membership on non-empty string table
    present = hashtable_contains(context->string_table, "five");
    if (present) {
        *err_str = "contains on non-empty string table failed";
        return false;
    }

    // Success
    return true;
}

static bool test_hashtable_duplicate_insertion(void * p_context, char ** err_str)
{
    hashtable_test_context_t context = (hashtable_test_context_t) p_context;
    bool success;

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // Initialize error string
    *err_str = NULL;

    // Insert once (should succeed)
    success = hashtable_insert(context->int_table, (void *) 5, "5 elem");
    if (!success) {
        *err_str = "first int insertion failed";
        return false;
    }

    // Insert again (should fail)
    success = hashtable_insert(context->int_table, (void *) 5, "5 elem");
    if (success) {
        *err_str = "second int insertion succeeded";
        return false;
    }

    // All tests passed!
    return true;
}

static bool test_hashtable_get(void * p_context, char ** err_str)
{
    hashtable_test_context_t context = (hashtable_test_context_t) p_context;
    bool success;
    hashtable_elem_t elem;

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // Initialize error string
    *err_str = NULL;

    // Insert an int
    success = hashtable_insert(context->int_table, (void *) 5, "5 elem");
    if (!success) {
        *err_str = "insertion failure";
        return false;
    }

    // Insert a string
    success = hashtable_insert(context->string_table, "five", "5 elem");
    if (!success) {
        *err_str = "insertion failure";
        return false;
    }

    // Get int member back
    elem = hashtable_get(context->int_table, (void *) 5);
    if (!elem || strcmp((char *) elem, "5 elem")) {
        *err_str = "get failed";
        return false;
    }

    // Get string member back
    elem = hashtable_get(context->string_table, "five");
    if (!elem || strcmp((char *) elem, "5 elem")) {
        *err_str = "get failed";
        return false;
    }

    // All tests passed!
    return true;
}

static bool test_hashtable_remove(void * p_context, char ** err_str)
{
    hashtable_test_context_t context = (hashtable_test_context_t) p_context;
    bool success;
    bool present;
    hashtable_elem_t elem;

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // Initialize error string
    *err_str = NULL;

    // Insert an int
    success = hashtable_insert(context->int_table, (void *) 5, "5 elem");
    if (!success) {
        *err_str = "int insertion failure";
        return false;
    }
    #ifdef VERBOSE
    printf("int insertion:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    // Insert a string
    success = hashtable_insert(context->string_table, "five", "5 elem");
    if (!success) {
        *err_str = "string insertion failure";
        return false;
    }
    #ifdef VERBOSE
    printf("string insertion:\n");
    hashtable_print(context->string_table);
    printf("\n");
    #endif

    // Check for int presence
    present = hashtable_contains(context->int_table, (void *) 5);
    if (!present) {
        *err_str = "int contains failure";
        return false;
    }

    // Check for int presence
    present = hashtable_contains(context->string_table, "five");
    if (!present) {
        *err_str = "string contains failure";
        return false;
    }

    // Remove int element
    elem = hashtable_remove(context->int_table, (void *) 5);
    if (!elem || strcmp((char *) elem, "5 elem")) {
        *err_str = "int remove failed";
        return false;
    }
    #ifdef VERBOSE
    printf("int removal:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    // Remove string element
    elem = hashtable_remove(context->string_table, "five");
    if (!elem || strcmp((char *) elem, "5 elem")) {
        *err_str = "string remove failed";
        return false;
    }
    #ifdef VERBOSE
    printf("string removal:\n");
    hashtable_print(context->string_table);
    printf("\n");
    #endif

    // Check for int presence
    present = hashtable_contains(context->int_table, (void *) 5);
    if (present) {
        *err_str = "int remove failure";
        return false;
    }

    // Check for int presence
    present = hashtable_contains(context->string_table, "five");
    if (present) {
        *err_str = "string remove failure";
        return false;
    }

    // All tests passed!
    return true;
}

static bool test_hashtable_stress(void * p_context, char ** err_str)
{
    hashtable_stress_context_t context = (hashtable_stress_context_t) p_context;
    uint32_t i;
    bool success;

    // Insert everything
    for (i = 0; i < N_STRESS_INSERTIONS; i++) {
        success = hashtable_insert(context->int_table, (void *)(uintptr_t) context->keys[i], context->elems[i]);
        if (!success) {
            *err_str = "int insertion failed";
            return false;
        }
    }

    #ifdef VERBOSE
    printf("all in:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    // Check that it's all there
    for (i = 0; i < N_STRESS_INSERTIONS; i++) {
        hashtable_elem_t elem = hashtable_get(context->int_table, (void *)(uintptr_t) context->keys[i]);
        if (!elem || strcmp((char *) elem, context->elems[i]) != 0) {
            printf("failed at %d\n", i);
            *err_str = "int retrieval failed";
            return false;
        }
    }

    // Remove everything
    for (i = 0; i < N_STRESS_INSERTIONS; i++) {
        hashtable_elem_t elem = hashtable_remove(context->int_table, (void *)(uintptr_t) context->keys[i]);
        if (!elem || strcmp((char *) elem, context->elems[i]) != 0) {
            printf("failed at %d\n", i);
            *err_str = "int retrieval failed";
            return false;
        }
    }

    #ifdef VERBOSE
    printf("all out:\n");
    hashtable_print(context->int_table);
    printf("\n");
    #endif

    // Check that nothing's there
    for (i = 0; i < N_STRESS_INSERTIONS; i++) {
        hashtable_elem_t elem = hashtable_get(context->int_table, (void *)(uintptr_t) context->keys[i]);
        if (elem) {
            printf("failed at %d\n", i);
            *err_str = "int retrieval failed";
            return false;
        }
    }

    // Success
    *err_str = "";
    return true;
}

static bool test_hashtable_threading(void * p_context, char ** err_str)
{
    (void) p_context;

    *err_str = "!!! unimplemented !!!";
    return true;
}
