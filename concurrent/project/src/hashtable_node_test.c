/**
 * @file    hashtable_node_test.c
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Unit test for hashtable nodes
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module under test
#include "hashtable_node.h"

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// Modules
#include "hashtable.h"
#include "unit_test.h"

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

typedef struct hashtable_node_test_context_t_ {
    hashtable_node_t zero;
    hashtable_node_t five;
    hashtable_node_t max;
} * hashtable_node_test_context_t;

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Initializes context to the standard struct
 */
static bool test_hashtable_node_standard_pre(void ** p_context, char ** err_str);

/**
 * @brief   Deallocates everything
 */
static void test_hashtable_node_standard_post(void * p_context);

/**
 * @brief   Tests creation and freeing of a hashtable node
 */
static bool test_hashtable_node_create(void * p_context, char ** err_str);

/**
 * @brief   Tests retrieving a hash
 */
static bool test_hashtable_node_get_hash(void * p_context, char ** err_str);

/**
 * @brief   Tests setting and retrieving an element
 */
static bool test_hashtable_node_get_set_elem(void * p_context, char ** err_str);

/**
 * @brief   Tests setting and retrieving the "next" field
 */
static bool test_hashtable_node_get_set_next(void * p_context, char ** err_str);

/**
 * @brief   Tests setting a node as a sentinel
 */
static bool test_hashtable_node_get_set_sentinel(void * p_context, char ** err_str);

/**
 * @brief   Tests CAS with elem field
 */
static bool test_hashtable_node_cas_elem(void * p_context, char ** err_str);

/**
 * @brief   Tests CAS with next field
 */
static bool test_hashtable_node_cas_next(void * p_context, char ** err_str);

/**
 * @brief   Tests first CAS function using the sentinel
 */
static bool test_hashtable_node_cas_sentinel_1(void * p_context, char ** err_str);

/**
 * @brief   Tests second CAS function using the sentinel
 */
static bool test_hashtable_node_cas_sentinel_2(void * p_context, char ** err_str);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

/**
 * @brief   Test entry point
 *
 * @return  1 if one or more tests failed, 0 otherwise
 */
int main(void)
{
    uint32_t err;
    unit_test_t hashtable_node_tests;

    // Allocate test structure
    hashtable_node_tests = unit_test_create("hashtable node");

    // Register tests
    unit_test_register(hashtable_node_tests,
                       "creation",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_create,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "hash retrieval",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_get_hash,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "elem storing",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_get_set_elem,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "next storing",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_get_set_next,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "sentinel",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_get_set_sentinel,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "elem cas",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_cas_elem,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "next cas",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_cas_next,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "sentinel cas 1",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_cas_sentinel_1,
                       test_hashtable_node_standard_post);
    unit_test_register(hashtable_node_tests,
                       "sentinel cas 2",
                       test_hashtable_node_standard_pre,
                       test_hashtable_node_cas_sentinel_2,
                       test_hashtable_node_standard_post);

    // Run tests
    if (unit_test_run(hashtable_node_tests)) err = 1;
    else                                err = 0;

    // Free test structure
    unit_test_free(hashtable_node_tests);

    return err;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static bool test_hashtable_node_standard_pre(void ** p_context, char ** err_str)
{
    hashtable_node_test_context_t context;

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!p_context) {
        *err_str = "!!! bad params !!!";
        return false;
    }

    // allocate context
    context = (hashtable_node_test_context_t) malloc(sizeof(struct hashtable_node_test_context_t_));
    if (!context) {
        *err_str = "test allocation failed";
        return false;
    }

    // allocate nodes
    context->zero = hashtable_node_create(NULL, 0);
    context->five = hashtable_node_create(NULL, 5);
    context->max = hashtable_node_create(NULL, UINT32_MAX);
    if (!context->zero || !context->five || !context->max) {
        hashtable_node_free(context->zero);
        hashtable_node_free(context->five);
        hashtable_node_free(context->max);
        free(context);
        *err_str = "test allocation failed";
        return false;
    }

    // Pass context on
    *p_context = (void *) context;

    *err_str = NULL;
    return true;
}

static void test_hashtable_node_standard_post(void * p_context)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // Free everything
    if (context) {
        hashtable_node_free(context->zero);
        hashtable_node_free(context->five);
        hashtable_node_free(context->max);

        free(context);
    }
}

static bool test_hashtable_node_create(void * p_context, char ** err_str)
{
    (void) p_context;

    // Actual test happens in preamble
    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_get_hash(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // Test 0 hash
    if (hashtable_node_get_hash(context->zero) != 0) {
        *err_str = "0 hash retrieval failed";
        return false;
    }

    // Test 5 hash
    if (hashtable_node_get_hash(context->five) != 5) {
        *err_str = "5 hash retrieval failed";
        return false;
    }

    // Test max hash
    if (hashtable_node_get_hash(context->max) != UINT32_MAX) {
        *err_str = "max hash retrieval failed";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_get_set_elem(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // Test 0 initial elem
    if (hashtable_node_get_elem(context->zero) != NULL) {
        *err_str = "0 initial elem retrieval failed";
        return false;
    }

    // Test 5 initial elem
    if (hashtable_node_get_elem(context->five) != NULL) {
        *err_str = "5 initial elem retrieval failed";
        return false;
    }

    // Test max initial elem
    if (hashtable_node_get_elem(context->max) != NULL) {
        *err_str = "max initial elem retrieval failed";
        return false;
    }

    // Set elems
    hashtable_node_set_elem(context->zero, (void *) 1);
    hashtable_node_set_elem(context->five, (void *) 2);
    hashtable_node_set_elem(context->max, (void *) UINTPTR_MAX - 1);

    // Test 0 set elem
    if (hashtable_node_get_elem(context->zero) != (void *) 1) {
        *err_str = "0 set elem retrieval failed";
        return false;
    }

    // Test 5 set elem
    if (hashtable_node_get_elem(context->five) != (void *) 2) {
        *err_str = "5 set elem retrieval failed";
        return false;
    }

    // Test max set elem
    if (hashtable_node_get_elem(context->max) != (void *) UINTPTR_MAX - 1) {
        *err_str = "max set elem retrieval failed";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_get_set_next(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // Test 0 initial next
    if (hashtable_node_get_next(context->zero) != NULL) {
        *err_str = "0 initial next retrieval failed";
        return false;
    }

    // Test 5 initial next
    if (hashtable_node_get_next(context->five) != NULL) {
        *err_str = "5 initial next retrieval failed";
        return false;
    }

    // Test max initial next
    if (hashtable_node_get_next(context->max) != NULL) {
        *err_str = "max initial next retrieval failed";
        return false;
    }

    // Set nexts
    hashtable_node_set_next(context->zero, context->five);
    hashtable_node_set_next(context->five, context->max);
    hashtable_node_set_next(context->max, context->zero);

    // Test 0 set next
    if (hashtable_node_get_next(context->zero) != context->five) {
        *err_str = "0 set next retrieval failed";
        return false;
    }

    // Test 5 set next
    if (hashtable_node_get_next(context->five) != context->max) {
        *err_str = "5 set next retrieval failed";
        return false;
    }

    // Test max set next
    if (hashtable_node_get_next(context->max) != context->zero) {
        *err_str = "max set next retrieval failed";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_get_set_sentinel(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // Test 0 initial sentinel
    if (hashtable_node_is_sentinel(context->zero)) {
        *err_str = "0 initial sentinel value failed";
        return false;
    }

    // Test 5 initial sentinel
    if (hashtable_node_is_sentinel(context->five)) {
        *err_str = "5 initial sentinel failed";
        return false;
    }

    // Test max initial sentinel
    if (hashtable_node_is_sentinel(context->max)) {
        *err_str = "max initial sentinel failed";
        return false;
    }

    // Set sentinels
    hashtable_node_set_sentinel(context->zero);
    hashtable_node_set_sentinel(context->five);
    hashtable_node_set_sentinel(context->max);

    // Test 0 set sentinel
    if (!hashtable_node_is_sentinel(context->zero)) {
        *err_str = "0 set sentinel retrieval failed";
        return false;
    }

    // Test 5 set sentinel
    if (!hashtable_node_is_sentinel(context->five)) {
        *err_str = "5 set sentinel retrieval failed";
        return false;
    }

    // Test max set sentinel
    if (!hashtable_node_is_sentinel(context->max)) {
        *err_str = "max set sentinel retrieval failed";
        return false;
    }

    // Set elems. Should clear sentinel
    hashtable_node_set_elem(context->zero, (void *) 0);
    hashtable_node_set_elem(context->five, (void *) 2);
    hashtable_node_set_elem(context->max, (void *) UINTPTR_MAX - 1);

    // Test 0 set sentinel
    if (hashtable_node_is_sentinel(context->zero)) {
        *err_str = "0 clear sentinel failed";
        return false;
    }

    // Test 5 set sentinel
    if (hashtable_node_is_sentinel(context->five)) {
        *err_str = "5 clear sentinel failed";
        return false;
    }

    // Test max set sentinel
    if (hashtable_node_is_sentinel(context->max)) {
        *err_str = "max clear sentinel failed";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_cas_elem(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // First attempted set. Should fail
    // first 0 cas
    if (hashtable_node_cas_elem(context->zero, (void *) 5, (void *) 10)) {
        *err_str = "first 0 cas succeeded when it shouldn't have";
        return false;
    }

    // first 5 cas
    if (hashtable_node_cas_elem(context->five, (void *) 42, (void *) 10)) {
        *err_str = "first 5 cas succeeded when it shouldn't have";
        return false;
    }

    // first max cas
    if (hashtable_node_cas_elem(context->max, (void *) UINTPTR_MAX - 1, (void *) 10)) {
        *err_str = "first max cas succeeded when it shouldn't have";
        return false;
    }

    // Second attempted set. Should succeed
    // second 0 cas
    if (!hashtable_node_cas_elem(context->zero, NULL, (void *) 1)) {
        *err_str = "second 0 cas failed";
        return false;
    }

    // second 5 cas
    if (!hashtable_node_cas_elem(context->five, NULL, (void *) 2)) {
        *err_str = "second 5 cas failed";
        return false;
    }

    // second max cas
    if (!hashtable_node_cas_elem(context->max, NULL, (void *) UINTPTR_MAX - 1)) {
        *err_str = "second max cas failed";
        return false;
    }

    // Check value is as expected
    // Retrieve 0 set elem
    if (hashtable_node_get_elem(context->zero) != (void *) 1) {
        *err_str = "0 set elem retrieval failed";
        return false;
    }

    // Retrieve 5 set elem
    if (hashtable_node_get_elem(context->five) != (void *) 2) {
        *err_str = "5 set elem retrieval failed";
        return false;
    }

    // Retrieve max set elem
    if (hashtable_node_get_elem(context->max) != (void *) UINTPTR_MAX - 1) {
        *err_str = "max set elem retrieval failed";
        return false;
    }

    // Third attempted CAS. Should fail
    // third 0 cas
    if (hashtable_node_cas_elem(context->zero, NULL, (void *) 11)) {
        *err_str = "third 0 cas succeeded";
        return false;
    }

    // third 5 cas
    if (hashtable_node_cas_elem(context->five, NULL, (void *) 42)) {
        *err_str = "third 5 cas succeeded";
        return false;
    }

    // third max cas
    if (hashtable_node_cas_elem(context->max, NULL, (void *) UINTPTR_MAX - 2)) {
        *err_str = "third max cas succeeded";
        return false;
    }

    // Check value is unchanged
    // Retrieve 0 set elem
    if (hashtable_node_get_elem(context->zero) != (void *) 1) {
        *err_str = "0 set elem retrieval failed";
        return false;
    }

    // Retrieve 5 set elem
    if (hashtable_node_get_elem(context->five) != (void *) 2) {
        *err_str = "5 set elem retrieval failed";
        return false;
    }

    // Retrieve max set elem
    if (hashtable_node_get_elem(context->max) != (void *) UINTPTR_MAX - 1) {
        *err_str = "max set elem retrieval failed";
        return false;
    }

    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_cas_next(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // First attempted set. Should fail
    // first 0 cas
    if (hashtable_node_cas_next(context->zero, context->max, context->five)) {
        *err_str = "first 0 cas succeeded when it shouldn't have";
        return false;
    }

    // first 5 cas
    if (hashtable_node_cas_next(context->five, context->zero, context->max)) {
        *err_str = "first 5 cas succeeded when it shouldn't have";
        return false;
    }

    // first max cas
    if (hashtable_node_cas_next(context->max, context->five, context->zero)) {
        *err_str = "first max cas succeeded when it shouldn't have";
        return false;
    }

    // Second attempted set. Should succeed
    // second 0 cas
    if (!hashtable_node_cas_next(context->zero, NULL, context->five)) {
        *err_str = "second 0 cas failed";
        return false;
    }

    // second 5 cas
    if (!hashtable_node_cas_next(context->five, NULL, context->max)) {
        *err_str = "second 5 cas failed";
        return false;
    }

    // second max cas
    if (!hashtable_node_cas_next(context->max, NULL, context->zero)) {
        *err_str = "second max cas failed";
        return false;
    }

    // Check value is as expected
    // Retrieve 0 set next
    if (hashtable_node_get_next(context->zero) != context->five) {
        *err_str = "0 set next retrieval failed";
        return false;
    }

    // Retrieve 5 set next
    if (hashtable_node_get_next(context->five) != context->max) {
        *err_str = "5 set next retrieval failed";
        return false;
    }

    // Retrieve max set next
    if (hashtable_node_get_next(context->max) != context->zero) {
        *err_str = "max set next retrieval failed";
        return false;
    }

    // Third attempted CAS. Should fail
    // third 0 cas
    if (hashtable_node_cas_next(context->zero, NULL, context->max)) {
        *err_str = "third 0 cas succeeded";
        return false;
    }

    // third 5 cas
    if (hashtable_node_cas_next(context->five, NULL, context->zero)) {
        *err_str = "third 5 cas succeeded";
        return false;
    }

    // third max cas
    if (hashtable_node_cas_next(context->max, NULL, context->five)) {
        *err_str = "third max cas succeeded";
        return false;
    }

    // Check value is unchanged
    // Retrieve 0 set next
    if (hashtable_node_get_next(context->zero) != context->five) {
        *err_str = "0 set next retrieval failed";
        return false;
    }

    // Retrieve 5 set next
    if (hashtable_node_get_next(context->five) != context->max) {
        *err_str = "5 set next retrieval failed";
        return false;
    }

    // Retrieve max set next
    if (hashtable_node_get_next(context->max) != context->zero) {
        *err_str = "max set next retrieval failed";
        return false;
    }

    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_cas_sentinel_1(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // First attempted set. Should fail
    // first 0 cas
    if (hashtable_node_if_sentinel_set_elem(context->zero, (void *) 1)) {
        *err_str = "first 0 cas succeeded when it shouldn't have";
        return false;
    }

    // first 5 cas
    if (hashtable_node_if_sentinel_set_elem(context->five, (void *) 2)) {
        *err_str = "first 5 cas succeeded when it shouldn't have";
        return false;
    }

    // first max cas
    if (hashtable_node_if_sentinel_set_elem(context->max, (void *) UINTPTR_MAX - 1)) {
        *err_str = "first max cas succeeded when it shouldn't have";
        return false;
    }

    // Make sentinels
    hashtable_node_set_sentinel(context->zero);
    hashtable_node_set_sentinel(context->five);
    hashtable_node_set_sentinel(context->max);

    // Second attempted set. Should succeed
    // second 0 cas
    if (!hashtable_node_if_sentinel_set_elem(context->zero, (void *) 1)) {
        *err_str = "second 0 cas failed";
        return false;
    }

    // second 5 cas
    if (!hashtable_node_if_sentinel_set_elem(context->five, (void *) 2)) {
        *err_str = "second 5 cas failed";
        return false;
    }

    // second max cas
    if (!hashtable_node_if_sentinel_set_elem(context->max, (void *) UINTPTR_MAX - 1)) {
        *err_str = "second max cas failed";
        return false;
    }

    // Check value is as expected
    // Retrieve 0 set elem
    if (hashtable_node_is_sentinel(context->zero) ||
        hashtable_node_get_elem(context->zero) != (void *) 1) {
        *err_str = "0 set elem retrieval failed";
        return false;
    }

    // Retrieve 5 set elem
    if (hashtable_node_is_sentinel(context->zero) ||
        hashtable_node_get_elem(context->five) != (void *) 2) {
        *err_str = "5 set elem retrieval failed";
        return false;
    }

    // Retrieve max set elem
    if (hashtable_node_is_sentinel(context->zero) ||
        hashtable_node_get_elem(context->max) != (void *) UINTPTR_MAX - 1) {
        *err_str = "max set elem retrieval failed";
        return false;
    }

    // Third attempted set. Should fail
    // third 0 cas
    if (hashtable_node_if_sentinel_set_elem(context->zero, (void *) 5)) {
        *err_str = "third 0 cas succeeded";
        return false;
    }

    // third 5 cas
    if (hashtable_node_if_sentinel_set_elem(context->five, (void *) 8)) {
        *err_str = "third 5 cas succeeded";
        return false;
    }

    // third max cas
    if (hashtable_node_if_sentinel_set_elem(context->max, (void *) UINTPTR_MAX - 2)) {
        *err_str = "third max cas succeeded";
        return false;
    }

    // Check value is as expected
    // Retrieve 0 set elem
    if (hashtable_node_is_sentinel(context->zero) ||
        hashtable_node_get_elem(context->zero) != (void *) 1) {
        *err_str = "0 set elem retrieval failed";
        return false;
    }

    // Retrieve 5 set elem
    if (hashtable_node_is_sentinel(context->zero) ||
        hashtable_node_get_elem(context->five) != (void *) 2) {
        *err_str = "5 set elem retrieval failed";
        return false;
    }

    // Retrieve max set elem
    if (hashtable_node_is_sentinel(context->zero) ||
        hashtable_node_get_elem(context->max) != (void *) UINTPTR_MAX - 1) {
        *err_str = "max set elem retrieval failed";
        return false;
    }

    *err_str = NULL;
    return true;
}

static bool test_hashtable_node_cas_sentinel_2(void * p_context, char ** err_str)
{
    hashtable_node_test_context_t context = (hashtable_node_test_context_t) p_context;

    // First attempted set. Should fail
    // first 0 cas
    if (hashtable_node_set_sentinel_if_elem(context->zero, (void *) 1)) {
        *err_str = "first 0 cas succeeded when it shouldn't have";
        return false;
    }

    // first 5 cas
    if (hashtable_node_set_sentinel_if_elem(context->five, (void *) 2)) {
        *err_str = "first 5 cas succeeded when it shouldn't have";
        return false;
    }

    // first max cas
    if (hashtable_node_set_sentinel_if_elem(context->max, (void *) UINTPTR_MAX - 1)) {
        *err_str = "first max cas succeeded when it shouldn't have";
        return false;
    }

    // Second attempted set. Should succeed
    // second 0 cas
    if (!hashtable_node_set_sentinel_if_elem(context->zero, NULL)) {
        *err_str = "second 0 cas failed";
        return false;
    }

    // second 5 cas
    if (!hashtable_node_set_sentinel_if_elem(context->five, NULL)) {
        *err_str = "second 5 cas failed";
        return false;
    }

    // second max cas
    if (!hashtable_node_set_sentinel_if_elem(context->max, NULL)) {
        *err_str = "second max cas failed";
        return false;
    }

    // Check value is as expected
    // check 0 sentinel
    if (!hashtable_node_is_sentinel(context->zero)) {
        *err_str = "0 set sentinel failed";
        return false;
    }

    // check 0 sentinel
    if (!hashtable_node_is_sentinel(context->five)) {
        *err_str = "5 set sentinel failed";
        return false;
    }

    // check 0 sentinel
    if (!hashtable_node_is_sentinel(context->max)) {
        *err_str = "max set sentinel failed";
        return false;
    }

    // Third attempted set. Should fail
    // third 0 cas
    if (hashtable_node_set_sentinel_if_elem(context->zero, NULL)) {
        *err_str = "third 0 cas succeeded";
        return false;
    }

    // third 5 cas
    if (hashtable_node_set_sentinel_if_elem(context->five, NULL)) {
        *err_str = "third 5 cas succeeded";
        return false;
    }

    // third max cas
    if (hashtable_node_set_sentinel_if_elem(context->max, NULL)) {
        *err_str = "third max cas succeeded";
        return false;
    }

    // Check value is as expected
    // check 0 sentinel
    if (!hashtable_node_is_sentinel(context->zero)) {
        *err_str = "0 set sentinel failed";
        return false;
    }

    // check 0 sentinel
    if (!hashtable_node_is_sentinel(context->five)) {
        *err_str = "5 set sentinel failed";
        return false;
    }

    // check 0 sentinel
    if (!hashtable_node_is_sentinel(context->max)) {
        *err_str = "max set sentinel failed";
        return false;
    }

    *err_str = NULL;
    return true;
}
