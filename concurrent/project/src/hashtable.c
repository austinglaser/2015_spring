/**
 * @file    hashtable.c
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implements a concurrent hashtable
 *
 * <Add more details>
 *
 * @addtogroup HASHTABLE
 * @{
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Module
#include "hashtable.h"

// Standard Libraries
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Node storing a single element in the hashtable
 */
typedef struct hashtable_node_t_ {
    hashtable_elem_t            elem;           /**< The stored element */
    size_t                      hash;           /**< The hash of the key associated with this element */
    struct hashtable_node_t_ *  next;           /**< The next element in the list (reverse hash ordering) */
} * hashtable_node_t

/**
 * @brief   The basic data structure for a hash table
 */
struct hashtable_t_ {
    size_t                      n_elements;     /**< The total number of elements stored in the table */
    size_t                      hash_size;      /**< The number of bits in the hash actually used for binning */
    hashtable_node_t *          hash_list;      /**< An array of hash bins, of length 2^<hash_size> */
};

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Allocates and returns a new hashtable node structure
 *
 * @param[in] elem:     The element for the structure
 * @param[in] hash:     The associated key hash
 *
 * @return:     An allocated hashtable node, or NULL if memory allocation failed
 */
static hashtable_node_t hashtable_node_create(hashtable_elem_t elem, size_t hash);

/**
 * @brief   De-allocates memory associated with a hashtable node
 *
 I @param[in] node:     The node to be freed
 */
static void hashtable_node_free(hashtable_node_t node);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

hashtable_t hashtable_create(hash_f_t hash_f)
{
    return NULL;
}

void hashtable_free(hashtable_t h)
{
}

bool hashtable_contains(hashtable_t h, hashtable_key_t key)
{
    return false;
}

hashtable_elem_t hashtable_get(hashtable_t h, hashtable_key_t key)
{
    return NULL;
}

hashtable_elem_t hashtable_remove(hashtable_t h, hashtable_key_t key)
{
    return NULL;
}

bool hashtable_insert(hashtable_t h, hashtable_key_t key, hashtable_elem_t val)
{
    return false;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static hashtable_node_t hashtable_node_create(hashtable_elem_t elem, size_t hash)
{
    // Allocate memory
    hashtable_node_t node = (hashtable_node_t) malloc(sizeof(struct hashtable_node_t_));
    if (!node) return NULL;

    // Initialize fields
    node->elem = elem;
    node->hash = hash;
    node->next = NULL;

    // Success
    return node;
}

static void hashtable_node_free(hashtable_node_t node)
{
    // Deallocate memory
    if (node) free(node);
}

/** @} addtogroup HASHTABLE */
