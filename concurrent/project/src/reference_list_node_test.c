/**
 * @file    reference_list_node_test.c
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Tests the reference list node class
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module under test
#include "reference_list_node.h"

// Standard Libraries
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Modules
#include "unit_test.h"

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

typedef struct reference_list_node_test_context_t_ {
    reference_list_node_t node_null;
    reference_list_node_t node_five;
} * reference_list_node_test_context_t;

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Initializes context to the standard struct
 */
static bool test_reference_list_node_standard_pre(void ** p_context, char ** err_str);

/**
 * @brief   Deallocates everything
 */
static void test_reference_list_node_standard_post(void * p_context);

/**
 * @brief   Tests creation and freeing of a reference list node
 */
static bool test_reference_list_node_create(void * p_context, char ** err_str);

/**
 * @brief   Tests retrieval of references from a node
 */
static bool test_reference_list_node_ref(void * p_context, char ** err_str);

/**
 * @brief   Tests operations on next field of the reference list node
 */
static bool test_reference_list_node_next(void * p_context, char ** err_str);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

/**
 * @brief   Test entry point
 *
 * @return  1 if one or more tests failed, 0 otherwise
 */
int main(void)
{
    uint32_t err;
    unit_test_t reference_list_node_tests;

    // Allocate test structure
    reference_list_node_tests = unit_test_create("reference list node");

    // Register tests
    unit_test_register(reference_list_node_tests,
                       "creation",
                       test_reference_list_node_standard_pre,
                       test_reference_list_node_create,
                       test_reference_list_node_standard_post);
    unit_test_register(reference_list_node_tests,
                       "reference retrieval",
                       test_reference_list_node_standard_pre,
                       test_reference_list_node_ref,
                       test_reference_list_node_standard_post);
    unit_test_register(reference_list_node_tests,
                       "get and set next",
                       test_reference_list_node_standard_pre,
                       test_reference_list_node_next,
                       test_reference_list_node_standard_post);

    // Run tests
    if (unit_test_run(reference_list_node_tests)) err = 1;
    else                                          err = 0;

    // Free test structure
    unit_test_free(reference_list_node_tests);

    return err;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static bool test_reference_list_node_standard_pre(void ** p_context, char ** err_str)
{

    // Ensure params are good
    if (!err_str) {
        return false;
    }
    if (!p_context) {
        *err_str = "!!! bad params !!!";

        return false;
    }

    // Allocate context
    reference_list_node_test_context_t context;
    context = (reference_list_node_test_context_t) malloc(sizeof(struct reference_list_node_test_context_t_));
    if (!context) {
        *err_str = "test allocation failed";
        return false;
    }
    *p_context = context;

    // Allocate reference
    uint32_t* five = (uint32_t*) malloc(sizeof(uint32_t));
    if (!five) {
        *err_str = "test allocation failed";
        return false;
    }
    *five = 5;

    // Allocate nodes
    context->node_null = reference_list_node_create(NULL);
    context->node_five = reference_list_node_create(five);
    if (!context->node_null || !context->node_five) {
        free(five);
        *err_str = "memory allocation failed";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}

static void test_reference_list_node_standard_post(void * p_context)
{
    reference_list_node_test_context_t context = (reference_list_node_test_context_t) p_context;

    // Free everything
    if (context) {
        if (context->node_five) {
            uint32_t* five = reference_list_node_get_ref(context->node_five);
            if (five) free(five);
            free(context->node_five);
        }
        if (context->node_null) free(context->node_null);

        free(context);
    }
}

static bool test_reference_list_node_create(void * p_context, char ** err_str)
{
    (void) p_context;

    // Actual test is in test setup. Success
    *err_str = NULL;
    return true;
}

static bool test_reference_list_node_ref(void * p_context, char ** err_str)
{
    reference_list_node_test_context_t context = (reference_list_node_test_context_t) p_context;
    void* null;

    // Try to get from NULL
    null = reference_list_node_get_ref(NULL);
    if (null) {
        *err_str = "get_ref from NULL should fail";
        return false;
    }

    // Get NULL back
    null = reference_list_node_get_ref(context->node_null);
    if (null) {
        *err_str = "get_ref from node_null should return NULL";
        return false;
    }

    // Get five back
    uint32_t* five = reference_list_node_get_ref(context->node_five);
    if (!five || (*five != 5)) {
        *err_str = "get_ref from node_five should return a reference to 5";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}

static bool test_reference_list_node_next(void * p_context, char ** err_str)
{
    reference_list_node_test_context_t context = (reference_list_node_test_context_t) p_context;
    bool success;
    reference_list_node_t next_five;
    reference_list_node_t next_null;

    // Check initial values
    next_five = reference_list_node_get_next(context->node_five);
    next_null = reference_list_node_get_next(context->node_null);
    if (next_five || next_null) {
        *err_str = "initial next values should be null";
        return false;
    }

    // Try to operate on NULL
    success = reference_list_node_set_next(NULL, context->node_five);
    if (success) {
        *err_str = "set next with NULL node should not succeed";
        return false;
    }

    // Try to set to NULL
    success = reference_list_node_set_next(context->node_null, NULL);
    if (success) {
        *err_str = "set next field to NULL should not succeed";
        return false;
    }

    // Verify unchanged
    next_null = reference_list_node_get_next(context->node_null);
    if (next_null) {
        *err_str = "invalid operation should not modify node next";
        return false;
    }

    // Actually set next
    success =            reference_list_node_set_next(context->node_null, context->node_five);
    success = success && reference_list_node_set_next(context->node_five, context->node_null);
    if (!success) {
        *err_str = "valid set should succeed";
        return false;
    }

    // Verify changed
    next_five = reference_list_node_get_next(context->node_five);
    next_null = reference_list_node_get_next(context->node_null);
    if ((next_five != context->node_null) || (next_null != context->node_five)) {
        *err_str = "get should retrieve the same value as a successful set";
        return false;
    }

    // Try to set again
    success =            reference_list_node_set_next(context->node_null, context->node_null);
    success = success || reference_list_node_set_next(context->node_five, context->node_five);
    if (success) {
        *err_str = "no sets after the first one should succeed";
        return false;
    }

    // Verify unchanged
    next_five = reference_list_node_get_next(context->node_five);
    next_null = reference_list_node_get_next(context->node_null);
    if ((next_five != context->node_null) || (next_null != context->node_five)) {
        *err_str = "invalid get shouldn't modify node";
        return false;
    }

    // Success
    *err_str = NULL;
    return true;
}
