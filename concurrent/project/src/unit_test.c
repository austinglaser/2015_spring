/**
 * @file    unit_test.h
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   A testing library for standard c
 *
 * <Add more details>
 *
 * @addtogroup UNIT_TEST
 * @{
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module
#include "unit_test.h"

// Standard Libraries
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <malloc.h>

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define RED     "\x1b[0;31;40m"
#define GREEN   "\x1b[0;32;40m"
#define RESET   "\x1b[0m"

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Structure for a single test
 */
typedef struct unit_test_node_t_ {
    char *                      name;       /**< The test's name */
    unit_test_f_t               code;       /**< The code used to execute the test */
    struct unit_test_node_t_ *  next;       /**< The next test to run */
} * unit_test_node_t;

/**
 * @brief   Unit test structure
 */
struct unit_test_t_ {
    uint32_t                    n_tests;    /**< How many tests are registered */
    unit_test_node_t            head;       /**< The linked list of tests */
};

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

unit_test_t unit_test_create(void)
{
    // Allocate memory
    unit_test_t tests = (unit_test_t) malloc(sizeof(struct unit_test_t_));
    if (!tests) return NULL;

    // Initialize fields
    tests->n_tests = 0;
    tests->head = NULL;

    return tests;
}

void unit_test_free(unit_test_t tests)
{
    unit_test_node_t curr;
    unit_test_node_t next;

    // Free test nodes
    curr = tests->head;
    while (curr) {
        next = curr->next;
        free(curr);
        curr = next;
    }

    // Free structure
    free(tests);
}

uint32_t unit_test_run(unit_test_t tests)
{
    char * err_str;
    bool passed;
    uint32_t n_failed = 0;
    unit_test_node_t curr;
    
    // Loop over all registered tests
    printf("\n");
    for (curr = tests->head; curr; curr = curr->next) {
        // Run test
        passed = curr->code(&err_str);

        // Print results
        if (passed) {
            printf("%-30s [ " GREEN "PASS" RESET " ]", curr->name);
        }
        else {
            printf("%-30s [ " RED "FAIL" RESET " ]", curr->name);
            n_failed++;
        }

        // Print error if there was one
        if (err_str && strlen(err_str) > 0) printf(": %s\n", err_str);
        else                                printf("\n");
    }
    printf("\n");

    return n_failed;
}

bool unit_test_register(unit_test_t tests, char * name, unit_test_f_t code)
{
    // Check input
    if (!tests || !name || !code) return false;

    // Allocate new node
    unit_test_node_t node = malloc(sizeof(struct unit_test_node_t_));
    if (!node) return false;
    node->name = name;
    node->code = code;

    // Check if list is empty
    if (tests->head == NULL) {
        // Debug check
        assert(tests->n_tests == 0);

        // Insert test
        tests->head = node;
        (tests->n_tests)++;
    }
    else {
        // Find the end
        unit_test_node_t curr;
        for (curr = tests->head; curr->next; curr = curr->next);

        // Insert test
        curr->next = node;
        (tests->n_tests)++;
    }

    return true;
}

/** @} addtogroup UNIT_TEST */
