/**
 * @file    hashtable_test.c
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Unit test for concurrent hashtable implementation
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module
#include "hashtable.h"

// Standard Libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define RED     "\x1b[0;31;40m"
#define GREEN   "\x1b[0;32;40m"
#define RESET   "\x1b[0m"

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Function signature for running a single test
 *
 * @note    The function should return true if the test passed, and false otherwise.
 *          If the parameter is set to NULL, or the empty string, it won't be printed. Otherwise,
 *          it will be printed as an error string
 */
typedef bool (*test_f_t)(char **);

/**
 * @brief   Structure for a single test
 */
typedef struct test_node_t_ {
    char *                  name;       /**< The test's name */
    test_f_t                code;       /**< The code used to execute the test */
    struct test_node_t_ *   next;       /**< The next test to run */
} * test_node_t;

/**
 * @brief   Unit test structure
 */
typedef struct test_t_ {
    uint32_t                n_tests;    /**< How many tests are registered */
    test_node_t             head;       /**< The linked list of tests */
} * test_t;

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Allocates a new test structure
 *
 * @return  A pointer to the new structure, or NULL if memory allocation failed
 */
test_t test_create(void);

/**
 * @brief   De-allocates a test structure
 *
 * @param[in] tests:    A pointer to the structure for deallocation
 */
void test_free(test_t tests);

/**
 * @brief   Runs all registered tests, prints the results
 *
 * @param[in] tests:    The test structure to run
 *
 * @return              The number of tests which failed
 */
uint32_t test_run_all(test_t tests);

/**
 * @brief   Registers a single test
 *
 * @param[in,out] tests:    The test structure to add to
 * @param[in] name:         A string encoding the name of the test
 * @param[in] code:         The test code to run
 *
 * @return  true if the insertion succeeded, false otherwise
 */
bool test_register(test_t tests, char * name, test_f_t code);

/**
 * @brief   Test hash function for an int
 */
size_t hash_int(hashtable_elem_t e);

/**
 * @brief   Test hash function for a string
 *
 * @note    Implementation from http://stackoverflow.com/questions/7666509/hash-function-for-string, http://www.cse.yorku.ca/~oz/hash.html
 */
size_t hash_string(hashtable_elem_t e);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

/**
 * @brief   Test entry point
 *
 * @return  -1 if one or more tests failed, 0 otherwise
 */
int main(void)
{
    uint32_t err;
    test_t hashtable_tests;

    hashtable_tests = test_create();

    // Register tests

    // Run tests
    if (test_run_all(hashtable_tests))  err = -1;
    else                                err = 0;

    // Free test structure
    test_free(hashtable_tests);

    return err;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

test_t test_create(void)
{
    // Allocate memory
    test_t tests = (test_t) malloc(sizeof(struct test_t_));
    if (!tests) return NULL;

    // Initialize fields
    tests->n_tests = 0;
    tests->head = NULL;

    return tests;
}

void test_free(test_t tests)
{
    test_node_t curr;
    test_node_t next;

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

uint32_t test_run_all(test_t tests)
{
    char * err_str;
    bool passed;
    uint32_t n_failed = 0;
    test_node_t curr;
    
    // Loop over all registered tests
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

    return n_failed;
}

bool test_register(test_t tests, char * name, test_f_t code)
{
    // Check input
    if (!tests || !name || !code) return false;

    // Allocate new node
    test_node_t node = malloc(sizeof(struct test_node_t_));
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
        test_node_t curr;
        for (curr = tests->head; curr->next; curr = curr->next);

        // Insert test
        curr->next = node;
        (tests->n_tests)++;
    }

    return true;
}

size_t hash_int(hashtable_elem_t e)
{
    return (size_t) e;
}

size_t hash_string(hashtable_elem_t e)
{
    size_t hash = 5381;
    uint32_t i;
    char * str = (char *) e;

    for (i = 0; str[i]; i++) hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */

    return hash;
}

