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
#include <assert.h>

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define HASHTABLE_INIT_SIZE     (2)             /**< The initial hash size */

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Node storing a single element in the hashtable
 */
typedef struct hashtable_node_t_ {
    hashtable_elem_t            elem;           /**< The stored element */
    size_t                      hash;           /**< The hash of the key associated with this element */
    struct hashtable_node_t_ *  next;           /**< The next element in the list (reverse hash ordering) */
    bool                        sentinel;       /**< Whether this is a sentinel node or not */
} * hashtable_node_t;

/**
 * @brief   The basic data structure for a hash table
 */
struct hashtable_t_ {
    size_t                      n_elements;     /**< The total number of elements stored in the table */
    size_t                      hash_size;      /**< The number of bits in the hash actually used for binning */
    hashtable_node_t *          hash_list;      /**< An array of hash bins, of length 2^<hash_size> */
    hashtable_node_t            head;           /**< The beginning of the reverse-hash-ordered list */
    hash_f_t                    hash_f;         /**< The function used to hash keys */
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
 * @param[in] node:     The node to be freed
 */
static void hashtable_node_free(hashtable_node_t node);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

hashtable_t hashtable_create(hash_f_t hash_f)
{
    uint32_t i;

    // Allocate memory
    hashtable_t h = (hashtable_t) malloc(sizeof(struct hashtable_t_));
    if (!h) return NULL;

    // Allcoate hash list
    h->hash_list = (hashtable_node_t*) malloc((1 << HASHTABLE_INIT_SIZE) * sizeof(hashtable_node_t));
    if (!h->hash_list) {
        free(h);

        // Failure
        return NULL;
    }

    // Allocate sentinel nodes
    for (i = 0; i < (1 << HASHTABLE_INIT_SIZE); i++) {
        h->hash_list[i] = hashtable_node_create(NULL, i);
        if (!h->hash_list[i]) {
            // Free all memory allocated to this point
            uint32_t j;
            for (j = 0; j < i; j++) hashtable_node_free(h->hash_list[j]);
            free(h->hash_list);
            free(h);
            return NULL;
        }

        h->hash_list[i]->sentinel = true;
    }

    // Initialize remaining fields
    h->n_elements = 0;
    h->hash_size = HASHTABLE_INIT_SIZE;
    h->hash_f = hash_f;

    // Build initial element list
    // TODO: make this flexible for different initial sizes
    assert(HASHTABLE_INIT_SIZE == 2);
    h->head = h->hash_list[0];
    h->hash_list[0]->next = h->hash_list[3];
    h->hash_list[3]->next = h->hash_list[1];
    h->hash_list[1]->next = h->hash_list[2];

    // Success
    return h;
}

void hashtable_free(hashtable_t h)
{
    hashtable_node_t curr;
    hashtable_node_t next;

    // Free element list
    curr = h->head;
    while (curr) {
        next = curr->next;
        hashtable_node_free(curr);
        curr = next;
    }

    // Free hash list
    free(h->hash_list);

    // Free table
    free(h);
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
    node->elem      = elem;
    node->hash      = hash;
    node->next      = NULL;
    node->sentinel  = false;

    // Success
    return node;
}

static void hashtable_node_free(hashtable_node_t node)
{
    // Deallocate memory
    if (node) free(node);
}

/** @} addtogroup HASHTABLE */
