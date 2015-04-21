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
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// Modules
#include "unit_test.h"

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_ELEMENTS(a)   (sizeof(a)/sizeof((a)[0]))

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Test hash function for an int
 */
static size_t hash_int(hashtable_elem_t e);

/**
 * @brief   Test hash function for a string
 *
 * @note    Implementation from http://stackoverflow.com/questions/7666509/hash-function-for-string, http://www.cse.yorku.ca/~oz/hash.html
 */
static size_t hash_string(hashtable_elem_t e);

/**
 * @brief   Tests creation of a hashtable
 */
static bool test_hashtable_create(char ** err_str);

/**
 * @brief   Tests insertion into a hashtable
 */
static bool test_hashtable_insert_contains(char ** err_str);

/**
 * @brief   Tests contains with an element not present
 */
static bool test_hashtable_contains_not_present(char ** err_str);

/**
 * @brief   Tests that multiple insertions are disallowed
 */
static bool test_hashtable_duplicate_insertion(char ** err_str);

/**
 * @brief   Insertion stress test
 */
static bool test_hashtable_insert_stress(char ** err_str);

/**
 * @brief   Tests getting an object back from the hashtable
 */
static bool test_hashtable_get(char ** err_str);

/**
 * @brief   Tests removing an object from the hashtable
 */
static bool test_hashtable_remove(char ** err_str);

/**
 * @brief   Test with threading
 *
 * @note    Unimplemented; will always fail. Probably will be several tests eventually
 */
static bool test_hashtable_threading(char ** err_str);

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
    hashtable_tests = unit_test_create();

    // Register tests
    unit_test_register(hashtable_tests, "creation",                         test_hashtable_create);
    unit_test_register(hashtable_tests, "insertion and membership",         test_hashtable_insert_contains);
    unit_test_register(hashtable_tests, "contains on non-present member",   test_hashtable_contains_not_present);
    unit_test_register(hashtable_tests, "duplicate insertion",              test_hashtable_duplicate_insertion);
    unit_test_register(hashtable_tests, "insert stress",                    test_hashtable_insert_stress);
    unit_test_register(hashtable_tests, "getting",                          test_hashtable_get);
    unit_test_register(hashtable_tests, "removing",                         test_hashtable_remove);
    unit_test_register(hashtable_tests, "threading",                        test_hashtable_threading);

    // Run tests
    if (unit_test_run(hashtable_tests)) err = -1;
    else                                err = 0;

    // Free test structure
    unit_test_free(hashtable_tests);

    return err;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static size_t hash_int(hashtable_elem_t e)
{
    return (size_t) e;
}

static size_t hash_string(hashtable_elem_t e)
{
    size_t hash = 5381;
    uint32_t i;
    char * str = (char *) e;

    for (i = 0; str[i]; i++) hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */

    return hash;
}

static bool test_hashtable_create(char ** err_str)
{
    // Allocation
    hashtable_t int_table       = hashtable_create(hash_int);
    hashtable_t string_table    = hashtable_create(hash_string);
    if (!int_table || !string_table) {
        // Indicate error
        *err_str = "memory allocation failed";
        return false;
    }

    // Free memory
    hashtable_free(int_table);
    hashtable_free(string_table);

    // Success
    *err_str = NULL;
    return true;
}

static bool test_hashtable_insert_contains(char ** err_str)
{
    bool success;
    bool present;

    // Allocation
    hashtable_t int_table       = hashtable_create(hash_int);
    hashtable_t string_table    = hashtable_create(hash_string);
    if (!int_table || !string_table) {
        // Indicate error
        *err_str = "memory allocation failed";
        return false;
    }

    // int insertion
    success = hashtable_insert(int_table, (void *) 5, "5 elem");
    if (!success) {
        *err_str = "int insertion failed";
        return false;
    }

    // string insertion
    success = hashtable_insert(string_table, "five", "5 elem");
    if (!success) {
        *err_str = "string insertion failed";
        return false;
    }

    // int membership
    present = hashtable_contains(int_table, (void *) 5);
    if (!present) {
        *err_str = "contains failed";
        return false;
    }

    // string membership
    present = hashtable_contains(string_table, "five");
    if (!present) {
        *err_str = "contains failed";
        return false;
    }

    // Free memory
    hashtable_free(int_table);
    hashtable_free(string_table);

    // Success
    *err_str = NULL;
    return true;
}

static bool test_hashtable_contains_not_present(char ** err_str)
{
    bool present;
    bool success;
    uint32_t i;
    size_t int_keys[] = {1, 2, 3, -4, 6, 10};
    char * string_keys[] = {"one", "two", "three", "negative four", "six", "ten"};
    char * elems[] = {"1 elem", "2 elem", "3 elem", "-4 elem", "6 elem", "10 elem"};

    // Allocation
    hashtable_t int_table       = hashtable_create(hash_int);
    hashtable_t string_table    = hashtable_create(hash_string);
    if (!int_table || !string_table) {
        // Indicate error
        *err_str = "memory allocation failed";
        return false;
    }

    // Membership on empty int table
    present = hashtable_contains(int_table, (void *) 5);
    if (present) {
        *err_str = "contains on empty int table failed";
        return false;
    }

    // Membership on empty string table
    present = hashtable_contains(string_table, "hi");
    if (present) {
        *err_str = "contains on empty string table failed";
        return false;
    }

    // Ensure arrays are the same size
    assert(ARRAY_ELEMENTS(int_keys)     == ARRAY_ELEMENTS(elems));
    assert(ARRAY_ELEMENTS(string_keys)  == ARRAY_ELEMENTS(elems));

    // Add some members to int table
    for (i = 0; i < ARRAY_ELEMENTS(int_keys); i++) {
        success = hashtable_insert(int_table, (void *) int_keys[i], elems[i]);
        if (!success) {
            *err_str = "insertion failed";
            return false;
        }
    }

    // Add some members to string table
    for (i = 0; i < ARRAY_ELEMENTS(string_keys); i++) {
        success = hashtable_insert(string_table, string_keys[i], elems[i]);
        if (!success) {
            *err_str = "insertion failed";
            return false;
        }
    }

    // Membership on non-empty int table
    present = hashtable_contains(int_table, (void *) 5);
    if (present) {
        *err_str = "contains on non-empty int table failed";
        return false;
    }

    // Membership on non-empty string table
    present = hashtable_contains(string_table, "hi");
    if (present) {
        *err_str = "contains on non-empty string table failed";
        return false;
    }

    // Free memory
    hashtable_free(int_table);
    hashtable_free(string_table);

    // Success
    *err_str = NULL;
    return true;
}

static bool test_hashtable_duplicate_insertion(char ** err_str)
{
    *err_str = "unimplemented!";
    return false;
}

static bool test_hashtable_insert_stress(char ** err_str)
{
    *err_str = "unimplemented!";
    return false;
}

static bool test_hashtable_get(char ** err_str)
{
    *err_str = "unimplemented!";
    return false;
}

static bool test_hashtable_remove(char ** err_str)
{
    *err_str = "unimplemented!";
    return false;
}

static bool test_hashtable_threading(char ** err_str)
{
    *err_str = "unimplemented!";
    return false;
}
