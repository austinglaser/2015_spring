/**
 * @file    hashtable.h
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implements a concurrent hashtable
 *
 * <Add more details>
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

/**
 * @defgroup HASHTABLE
 * @{
 */

/* --- PUBLIC DEPENDENCIES -------------------------------------------------- */

// Standard Libraries
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* --- PUBLIC DATA TYPES ---------------------------------------------------- */

/**
 * @brief   The main API data type
 */
typedef struct hashtable_t_ * hashtable_t;

/**
 * @brief   Data used as keys are generic pointers
 */
typedef void * hashtable_key_t;

/**
 * @brief   Data stored are generic pointers
 */
typedef void * hashtable_elem_t;

/**
 * @brief   Function signature for hashing key objects
 */
typedef uint32_t (*hash_f_t)(hashtable_key_t);

/**
 * @brief   Function signature for printing elements
 */
typedef void (*print_f_t)(hashtable_elem_t);

/**
 * @brief   Function signature for freeing elements
 */
typedef void (*free_f_t)(hashtable_elem_t);

/* --- PUBLIC FUNCTION PROTOTYPES ------------------------------------------- */

/**
 * @brief   Allocates and returns a new hashtable object
 *
 * Takes no size parameter; the returned hashtable is of
 * (algorithmically) unlimited size, but may be resized at
 * runtime for performance purposes. 
 *
 * @param[in] hash_f:   A function which will return a hash for a table value.
 *                      If a table which can store multiple types of objects is desired,
 *                      the function must be able to hash any and all of them appropriately
 *
 * @return              A new hashtable object, or NULL if memory allocation fails
 */
hashtable_t hashtable_create(hash_f_t hash_f, print_f_t print_f, free_f_t free_f);

/**
 * @brief   Deletes the hashtable, de-allocating all memory used
 *
 * @param[in] h:        The hashtable to be freed
 */
void hashtable_free(hashtable_t h);

/**
 * @brief   Returns true if the hashtable has a value corresponding to <key>, false otherwise
 *
 * @param[in] h:        The hashtable to search
 * @param[in] key:      A piece of data, hashable with the hash function provide for <h>
 * 
 * @return              True if <h>[<key>] contains an value, false otherwise
 */
bool hashtable_contains(hashtable_t h,
                        hashtable_key_t key);

/**
 * @brief   Inserts <val> at <h>[<key>]
 *
 * @param[in,out] h:    The hashtable to modify
 * @param[in] key:      The value to use as a key
 * @param[in] val:      The data to store at <h>[<key>]
 *
 * @return              True if the data was successfuly inserted. False if there is already an element at <h>[<key>]
 */
bool hashtable_insert(hashtable_t h,
                      hashtable_key_t key,
                      hashtable_elem_t val);

/**
 * @brief   Gets the value at <h>[<key>], leaving that object in the table
 *
 * @param[in] h:        The hashtable to search
 * @param[in] key:      A piece of data, hashable with the hash function provide for <h>
 * 
 * @return              The object residing at <h>[<key>], or NULL if none exists
 */
hashtable_elem_t hashtable_get(hashtable_t h,
                               hashtable_key_t key);

/**
 * @brief   Removes the value at <h>[<key>], and returns it
 *
 * @param[in] h:        The hashtable to search
 * @param[in] key:      A piece of data, hashable with the hash function provide for <h>
 * 
 * @return              The object residing at <h>[<key>], or NULL if none exists
 */
hashtable_elem_t hashtable_remove(hashtable_t h,
                                  hashtable_key_t key);

/**
 * @brief   Prints a hashtable
 *
 * @param[in] h:        The hashtable to print
 */
void hashtable_print(hashtable_t h);

/** @} defgroup HASHTABLE */

#endif // ifndef HASHTABLE_H_
