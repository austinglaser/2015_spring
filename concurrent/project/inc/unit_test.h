/**
 * @file    unit_test.h
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implements a concurrent unit_test
 *
 * <Add more details>
 */

#ifndef UNIT_TEST_H_
#define UNIT_TEST_H_

/**
 * @defgroup UNIT_TEST
 * @{
 */

/* --- PUBLIC DEPENDENCIES -------------------------------------------------- */

// Standard Libraries
#include <stdint.h>
#include <stdbool.h>

/* --- PUBLIC DATA TYPES ---------------------------------------------------- */

/**
 * @brief   Unit test dat structure
 */
typedef struct unit_test_t_ * unit_test_t;

/**
 * @brief   Function signature for pre-test tasks
 *
 * If this returns false, the test is considered a failure
 * and the rest is not run. It should initialize the
 * first argument to a pointer to the 'context', containing
 * variables and information for running the rest of the test
 */
typedef bool (*unit_test_pre_f_t)(void **, char **);

/**
 * @brief   Function signature for the body of a test
 *
 * The function should return true if the test passed, and false otherwise.
 * If the parameter is set to NULL, or the empty string, it won't be printed. Otherwise,
 * it will be printed as an error string
 */
typedef bool (*unit_test_body_f_t)(void *, char **);

/**
 * @brief   Function signature for post-test tasks
 *
 * This should clean up and deallocate any memory allocated
 * throughout the rest of the test. It does not have any effect on
 * a test's success/failure
 */
typedef void (*unit_test_post_f_t)(void *);

/* --- PUBLIC FUNCTION PROTOTYPES ------------------------------------------- */

/**
 * @brief   Allocates a new test structure
 *
 * @param[in] test_name:    The overall name of this unit test
 *
 * @return  A pointer to the new structure, or NULL if memory allocation failed
 */
unit_test_t unit_test_create(char * test_name);

/**
 * @brief   De-allocates a test structure
 *
 * @param[in] tests:    A pointer to the structure for deallocation
 */
void unit_test_free(unit_test_t tests);

/**
 * @brief   Runs all registered tests, prints the results
 *
 * @param[in] tests:    The test structure to run
 *
 * @return              The number of tests which failed
 */
uint32_t unit_test_run(unit_test_t tests);

/**
 * @brief   Registers a single test
 *
 * @param[in,out] tests:    The test structure to add to
 * @param[in] name:         A string encoding the name of the test
 * @param[in] code:         The test code to run
 *
 * @return  true if the insertion succeeded, false otherwise
 */
bool unit_test_register(unit_test_t tests, char * name, unit_test_pre_f_t pre, unit_test_body_f_t code, unit_test_post_f_t post);

/** @} defgroup UNIT_TEST */

#endif // ifndef UNIT_TEST_H_
