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

#define HASH_WIDTH_INIT         (2)             /**< The initial hash size */

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Node storing a single element in the hashtable
 */
typedef struct hashtable_node_t_ {
    hashtable_elem_t            elem;           /**< The stored element */
    uint32_t                    hash;           /**< The hash of the key associated with this element */
    struct hashtable_node_t_ *  next;           /**< The next element in the list (reverse hash ordering) */
    bool                        sentinel;       /**< Whether this is a sentinel node or not */
} * hashtable_node_t;

/**
 * @brief   The basic data structure for a hash table
 */
struct hashtable_t_ {
    uint32_t                    n_elements;     /**< The total number of elements stored in the table */
    uint32_t                    hash_width;     /**< The number of bits in the hash actually used for binning */
    uint32_t                    hash_mask;      /**< The mask used to determine the significant bits in a hash value */
    hashtable_node_t *          hash_list;      /**< An array of hash bins, of length 2^<hash_width> */
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
static hashtable_node_t hashtable_node_create(hashtable_elem_t elem, uint32_t hash);

/**
 * @brief   De-allocates memory associated with a hashtable node
 *
 * @param[in] node:     The node to be freed
 */
static void hashtable_node_free(hashtable_node_t node);

/**
 * @brief   Bit reverses <val>
 *
 * @note    from <https://graphics.stanford.edu/~seander/bithacks.html#BitReverseTable>
 *
 * @param[in] val:  The value to be bit-reversed
 *
 * @return:         <val>, bit-reversed
 */
static inline uint32_t hashtable_uint32_bit_reverse(uint32_t val);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

hashtable_t hashtable_create(hash_f_t hash_f)
{
    uint32_t i;

    // Allocate memory
    hashtable_t h = (hashtable_t) malloc(sizeof(struct hashtable_t_));
    if (!h) return NULL;

    // Allcoate hash list
    h->hash_list = (hashtable_node_t*) malloc((1 << HASH_WIDTH_INIT) * sizeof(hashtable_node_t));
    if (!h->hash_list) {
        free(h);

        // Failure
        return NULL;
    }

    // Allocate sentinel nodes
    for (i = 0; i < (1 << HASH_WIDTH_INIT); i++) {
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
    h->n_elements   = 0;
    h->hash_width   = HASH_WIDTH_INIT;
    h->hash_f       = hash_f;
    h->hash_mask    = 0x00000000;
    for (i = 0; i < h->hash_width; i++) h->hash_mask |= 0x01 << i;

    // Build initial element list
    // TODO: make this flexible for different initial widths
    assert(HASH_WIDTH_INIT == 2);
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
    hashtable_node_t prev;
    hashtable_node_t curr;
    uint32_t hash;
    uint32_t reversed;

    // Generate hash
    hash = h->hash_f(key);
    reversed = hashtable_uint32_bit_reverse(hash);

    // Search table
    curr = h->hash_list[hash & h->hash_mask];
    reversed = hashtable_uint32_bit_reverse(hash);
    while (curr && hashtable_uint32_bit_reverse(curr->hash) < reversed) {
        prev = curr;
        curr = prev->next;
    }

    // Check if hash is already present
    if (curr && curr->hash == hash && !curr->sentinel) return false;
    else                                               return true;
}

bool hashtable_insert(hashtable_t h, hashtable_key_t key, hashtable_elem_t elem)
{
    hashtable_node_t prev;
    hashtable_node_t curr;
    hashtable_node_t node;
    uint32_t hash;
    uint32_t reversed;

    // Check input
    if (!h) return false;

    // Determine whether to resize
    if ((h->n_elements + 1) > ((1U << h->hash_width)*2)) {
        // Resize
        hashtable_node_t* temp = (hashtable_node_t*) realloc(h->hash_list, ((1 << h->hash_width)*2));
        if (!temp) return false;
        h->hash_list = temp;

        // Create references to the new list locations
        uint32_t i;
        for (i = (1U << h->hash_width); i < (1U << h->hash_width)*2; i++) {
            // Step through the list until we find the location we're looking for
            curr = h->head;
            reversed = hashtable_uint32_bit_reverse(i);
            while (curr && hashtable_uint32_bit_reverse(curr->hash) < reversed) {
                prev = curr;
                curr = prev->next;
            }

            // Check if we should create a new node
            if (curr && i == curr->hash) {
                // Just set our reference
                h->hash_list[i] = curr;
            }
            else {
                // Create a sentinel node
                node = hashtable_node_create(NULL, i);
                node->sentinel = true;

                // Insert it
                node->next = curr;
                prev->next = node;

                // Set the reference
                h->hash_list[i] = curr;
            }
        }

        // Increase hash width
        h->hash_mask |= (1 << h->hash_width);
        (h->hash_width)++;
    }

    // Get the key's hash
    hash = h->hash_f(key);

    // Find the appropriate place in the table
    curr = h->hash_list[hash & h->hash_mask];
    reversed = hashtable_uint32_bit_reverse(hash);
    while (curr && hashtable_uint32_bit_reverse(curr->hash) < reversed) {
        prev = curr;
        curr = prev->next;
    }

    // Check if hash is already present
    if (curr && curr->hash == hash) {
        // See if it's a sentinel
        if (!curr->sentinel) return false;
        else {
            curr->sentinel = false;
            curr->elem = elem;
        }
    }
    else {
        // Create a new node
        node = hashtable_node_create(elem, hash);

        // Insert it
        node->next = curr;
        prev->next = node;
    }

    // Success
    return true;
}

hashtable_elem_t hashtable_get(hashtable_t h, hashtable_key_t key)
{
    return NULL;
}

hashtable_elem_t hashtable_remove(hashtable_t h, hashtable_key_t key)
{
    return NULL;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static hashtable_node_t hashtable_node_create(hashtable_elem_t elem, uint32_t hash)
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

static inline uint32_t hashtable_uint32_bit_reverse(uint32_t val)
{
    // Lookup table per byte
    #define R2(n)     (n),   ((n) | (2 << 6)),   ((n) | (1 << 6)),   ((n) | (3 << 6))
    #define R4(n)   R2(n), R2((n) | (2 << 4)), R2((n) | (1 << 4)), R2((n) | (3 << 4))
    #define R6(n)   R4(n), R4((n) | (2 << 2)), R4((n) | (1 << 2)), R4((n) | (3 << 2))
    static const uint8_t reversed_uint8[256] = {
        R6(0), R6(2), R6(1), R6(3)
    };

    uint32_t reversed;
    uint8_t * reversed_p    = (uint8_t*) &reversed;
    uint8_t * val_p         = (uint8_t*) &val;

    // Reverse each byte, and its position
    reversed_p[3] = reversed_uint8[val_p[0]];
    reversed_p[2] = reversed_uint8[val_p[1]];
    reversed_p[1] = reversed_uint8[val_p[2]];
    reversed_p[0] = reversed_uint8[val_p[3]];

    // Return reversed value
    return reversed;
}

/** @} addtogroup HASHTABLE */
