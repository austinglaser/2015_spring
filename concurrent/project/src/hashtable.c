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
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <assert.h>
#include <string.h>

// Other modules
#include "hashtable_node.h"
#include "reference_list.h"
 
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define HASH_WIDTH_INIT         (2)             /**< The initial hash size */
#define SAVE_SLOTS_INIT         (256)           /**< The initial number of pointer/node saving slots */

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   The basic data structure for a hash table
 */
struct hashtable_t_ {
    atomic_uint_fast32_t        n_elements;                 /**< The total number of elements stored in the table */
    uint32_t                    hash_width;                 /**< The number of bits in the hash actually used for binning */
    uint32_t                    hash_mask;                  /**< The mask used to determine the significant bits in a hash value */
    hashtable_node_t *          hash_list;                  /**< An array of hash bins, of length 2^<hash_width> */
    atomic_flag                 table_resizing;             /**< A thread must acquire this flag to resize the hashtable */
    hash_f_t                    hash_f;                     /**< The function used to hash keys */
    print_f_t                   print_f;                    /**< The function used to print elements */
    free_f_t                    free_f;                     /**< The function used to free elements */
    reference_list_t            saved_nodes;                /**< A list of saved hashtable nodes */
    reference_list_t            saved_pointers;             /**< A list of pointers which can be deallocated with free() alone */
};

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Looks for a node with a given hash
 *
 * Steps through the hashtable list until it finds a node with hash. If that node is in the table, curr
 * will point to it at return. If it isn't, curr will point to the node after where it should go,
 * and prev the one before it
 *
 * @param[in] h:            The hashtable to search
 * @param[in] hash:         The hash to search for
 * @param[out] curr:        Will point to a node with the given hash, or the one after its spot
 * @param[out] prev:        Points to the node before curr
 */
static inline void hashtable_find_location(hashtable_t h, uint32_t hash, hashtable_node_t * curr, hashtable_node_t * prev);

/**
 * @brief   Saves a node for later deallocation
 *
 * @param[in,out] h:    The corresponding hashtable object
 * @param[in] node:     The node to save
 */
static inline void hashtable_save_node(hashtable_t h, hashtable_node_t node);

/**
 * @brief   Saves a pointer for later deallocdation
 *
 * @note    This function should only be used to save data which can be
 *          deallocated with a single call to free()
 *
 * @param[in,out] h:    The corresponding hashtabble object
 * @param[in] ptr:      The pointer to save
 */
static inline void hashtable_save_pointer(hashtable_t h, void * ptr);

/**
 * @brief   Resizes the array pointed to by array_ptr from old_size to new_size
 *
 * @param[in, out] h:               The corresponding hashtable object
 * @param[in,out] array:            The array to be modified
 * @param[in] old_size:             The current number of elements in the array
 * @param[in] new_size:             The desired number of elements in the array
 * @param[in] elem_size:            The size (in bytes) of a single array element
 *
 * @return      true if successful, false otherwise
 */
static inline bool hashtable_resize_array(hashtable_t h, void ** array_ptr, uint32_t old_size, uint32_t new_size, uint32_t elem_size);

/**
 * @brief   Wrapper for hashtable_node_free
 *
 * @see hashtable_node_free
 */
static void hashtable_node_reference_list_free(void* elem);

/**
 * @brief   Bit reverses <val>
 *
 * @note    from <https://graphics.stanford.edu/~seander/bithacks.html#BitReverseTable>
 *
 * @param[in] val:      The value to be bit-reversed
 *
 * @return:         <val>, bit-reversed
 */
static inline uint32_t hashtable_uint32_bit_reverse(uint32_t val);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

hashtable_t hashtable_create(hash_f_t hash_f, print_f_t print_f, free_f_t free_f)
{
    uint_fast32_t i;

    // Allocate memory
    hashtable_t h = (hashtable_t) malloc(sizeof(struct hashtable_t_));
    if (!h) return NULL;

    // Initialize pointer fields
    h->hash_list = NULL;
    h->saved_nodes = NULL;
    h->saved_pointers = NULL;

    // Allcoate hash list
    h->hash_list = (hashtable_node_t*) malloc((1 << HASH_WIDTH_INIT) * sizeof(hashtable_node_t));
    if (!h->hash_list) {
        // Clean up struct
        hashtable_free(h);

        // Failure
        return NULL;
    }
    // Fill with zeros
    memset(h->hash_list, 0, (1 << HASH_WIDTH_INIT) * sizeof(hashtable_node_t));
    
    // Create reference saving arrays
    h->saved_nodes      = reference_list_create(hashtable_node_reference_list_free);
    h->saved_pointers   = reference_list_create(free);
    if (!h->saved_nodes || !h->saved_pointers) {
        if (h->saved_nodes)    reference_list_free(h->saved_nodes);
        if (h->saved_pointers) reference_list_free(h->saved_pointers);

        // Clean up struct
        hashtable_free(h);

        // Failure
        return NULL;
    }

    // Allocate sentinel nodes
    hashtable_node_t tmp_node[(1 << HASH_WIDTH_INIT)];
    for (i = 0; i < (1 << HASH_WIDTH_INIT); i++) {
        tmp_node[i] = hashtable_node_create(NULL, i);
        if (!(tmp_node[i])) {
            // Free all memory allocated to this point
            uint_fast32_t j;
            for (j = 0; j < i; j++) hashtable_node_free(tmp_node[j]);

            // Clean up struct
            hashtable_free(h);

            // Failure
            return NULL;
        }

        hashtable_node_set_sentinel(tmp_node[i]);
        h->hash_list[i] = tmp_node[i];
    }

    // Initialize remaining fields
    h->hash_width   = HASH_WIDTH_INIT;
    h->hash_f       = hash_f;
    h->print_f      = print_f;
    h->free_f       = free_f;
    atomic_flag_clear(&(h->table_resizing));
    atomic_init(&(h->n_elements), 0);
    h->hash_mask    = 0x00000000;
    for (i = 0; i < h->hash_width; i++) h->hash_mask |= 0x01 << i;

    // Build initial element list
    // TODO: make this flexible for different initial widths
    assert(HASH_WIDTH_INIT == 2);
    hashtable_node_set_next(h->hash_list[0], h->hash_list[2]);
    hashtable_node_set_next(h->hash_list[2], h->hash_list[1]);
    hashtable_node_set_next(h->hash_list[1], h->hash_list[3]);

    // Success
    return h;
}

void hashtable_free(hashtable_t h)
{
    hashtable_node_t curr;
    hashtable_node_t next;

    if (h) {
        // Free element list
        curr = h->hash_list[0];
        while (curr) {
            next = hashtable_node_get_next(curr);
            if (h->free_f) h->free_f(hashtable_node_get_elem(curr));
            hashtable_node_free(curr);
            curr = next;
        }

        // Free hash list
        if (h->hash_list) free(h->hash_list);

        // Free all saved references
        if (h->saved_nodes)    reference_list_free(h->saved_nodes);
        if (h->saved_pointers) reference_list_free(h->saved_pointers);

        // Free table
        free(h);
    }
}

bool hashtable_contains(hashtable_t h, hashtable_key_t key)
{
    // Get the element
    hashtable_elem_t dummy = hashtable_get(h, key);

    // If it's not null, it's in the table
    return dummy != NULL;
}

bool hashtable_insert(hashtable_t h, hashtable_key_t key, hashtable_elem_t elem)
{
    hashtable_node_t prev;
    hashtable_node_t curr;
    hashtable_node_t node;

    // Check input
    if (!h) return false;

    // If some other thread isn't already resizing, we'll do it
    bool already_resizing = atomic_flag_test_and_set(&(h->table_resizing));
    if (!already_resizing) {
        if ((atomic_load(&(h->n_elements)) + 1) > ((1U << h->hash_width)*2)) {
            // Resize
            if (hashtable_resize_array(h, (void **) &(h->hash_list), (1 << h->hash_width), (1 << h->hash_width)*2, sizeof(hashtable_node_t*))) {
                // Create references to the new list locations
                uint_fast32_t i;
                for (i = (1U << h->hash_width); i < (1U << h->hash_width)*2; i++) {
                    while (true) {
                        hashtable_find_location(h, i, &curr, &prev);

                        // Check if we should create a new node
                        if (curr && i == hashtable_node_get_hash(curr)) {
                            // Just set our reference
                            h->hash_list[i] = curr;

                            // Done with this sentinel
                            break;
                        }
                        else {
                            // Create a sentinel node
                            node = hashtable_node_create(NULL, i);
                            hashtable_node_set_sentinel(node);

                            // Insert it
                            hashtable_node_set_next(node, curr);
                            if (hashtable_node_cas_next(prev, curr, node)) {
                                // Set the reference
                                h->hash_list[i] = node;

                                // Done with this sentinel
                                break;
                            }
                            else {
                                hashtable_node_free(node);
                            }
                        }
                    }
                }

                // Increase hash width
                h->hash_mask |= (1 << h->hash_width);
                (h->hash_width)++;
            }
        }
    }
    atomic_flag_clear(&(h->table_resizing));

    // Get the key's hash
    uint32_t hash;
    hash = h->hash_f(key);

    // Loop until success
    bool insert_success = false;
    do {
        // Find the appropriate place in the table
        hashtable_find_location(h, hash, &curr, &prev);

        // Check if hash is already present
        if (curr && hashtable_node_get_hash(curr) == hash) {
            // See if it's a sentinel
            if (!hashtable_node_is_sentinel(curr)) return false;

            // If it's still a sentinel, set the element
            insert_success = hashtable_node_if_sentinel_set_elem(curr, elem);
        }
        else {
            // Create a new node
            node = hashtable_node_create(elem, hash);

            // Insert it
            hashtable_node_set_next(node, curr);
            insert_success = hashtable_node_cas_next(prev, curr, node);

            // Clean up after ourselves
            if (!insert_success) free(elem);
        }
    } while (!insert_success);

    // Increase element count
    atomic_fetch_add(&(h->n_elements), 1);

    // Success
    return true;
}

hashtable_elem_t hashtable_get(hashtable_t h, hashtable_key_t key)
{
    hashtable_node_t prev;
    hashtable_node_t curr;
    uint32_t hash;

    // Generate hash
    hash = h->hash_f(key);

    // Search table
    hashtable_find_location(h, hash, &curr, &prev);

    // Check if hash is already present
    if (curr && hashtable_node_get_hash(curr) == hash && !hashtable_node_is_sentinel(curr)) {
        return hashtable_node_get_elem(curr);
    }
    else {
        return NULL;
    }
}

hashtable_elem_t hashtable_remove(hashtable_t h, hashtable_key_t key)
{
    hashtable_node_t prev;
    hashtable_node_t curr;
    uint32_t hash;

    // Generate hash
    hash = h->hash_f(key);

    // Loop until successful removal
    hashtable_elem_t elem;
    bool remove_success = false;
    do {
        // Search table
        hashtable_find_location(h, hash, &curr, &prev);

        // Check if it's actually in the table
        uint32_t node_hash = hashtable_node_get_hash(curr);
        if (curr && node_hash == hash && !hashtable_node_is_sentinel(curr)) {
            // Determine if it should be left in as a sentinel
            if (node_hash == (node_hash & h->hash_mask)) {
                // Save the element
                elem = hashtable_node_get_elem(curr);

                // Try to set as sentinel
                remove_success = hashtable_node_set_sentinel_if_elem(curr, elem);
            }
            else {
                // Save the element
                elem = hashtable_node_get_elem(curr);

                // Try to remove from the list
                remove_success = hashtable_node_cas_next(prev, curr, hashtable_node_get_next(curr));

                // Save the node for later deallocation
                if (remove_success) hashtable_save_node(h, curr);
            }
        }
        else {
            // Not present
            return NULL;
        }
    } while (!remove_success);

    // Decrement the number of elements
    atomic_fetch_sub(&(h->n_elements), 1);

    // Pass back the element
    return elem;
}

void hashtable_print(hashtable_t h)
{
    hashtable_node_t curr;

    for (curr = h->hash_list[0]; curr; curr = hashtable_node_get_next(curr)) {
        uint32_t hash = hashtable_node_get_hash(curr);
        if (hashtable_node_is_sentinel(curr)) {
            printf("[ ...0x%08x (0x%08x) ]\n", hash, hashtable_uint32_bit_reverse(hash));
        }
        else {
            printf("[    0x%08x (0x%08x) ]: ", hash, hashtable_uint32_bit_reverse(hash));
            h->print_f(hashtable_node_get_elem(curr));
            printf("\n");
        }
    }
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static inline void hashtable_find_location(hashtable_t h, uint32_t hash, hashtable_node_t * curr, hashtable_node_t * prev)
{
    // Get reversed hash
    uint32_t reversed = hashtable_uint32_bit_reverse(hash);
    
    // Find start node
    *curr = h->hash_list[hash & h->hash_mask];

    // Step through the list
    while (*curr && hashtable_uint32_bit_reverse(hashtable_node_get_hash(*curr)) < reversed) {
        *prev = *curr;
        *curr = hashtable_node_get_next(*prev);
    }
}

static inline void hashtable_save_node(hashtable_t h, hashtable_node_t node)
{
    // Shove it in the list
    reference_list_insert(h->saved_nodes, node);
}

static inline void hashtable_save_pointer(hashtable_t h, void * pointer)
{
    // Shove it in the list
    reference_list_insert(h->saved_pointers, pointer);
}

static inline bool hashtable_resize_array(hashtable_t h, void ** array, uint32_t old_size, uint32_t new_size, uint32_t elem_size)
{
    // Check inputs
    if (!array || !*array) return false;

    // Allocate new memory
    void * new_array = (void *) malloc(new_size*elem_size);
    if (!new_array) return false;

    // Copy data
    memcpy(new_array, *array, old_size*elem_size);

    // Swap over reference
    void * old_array = *array;
    *array = new_array;

    // Save old array
    hashtable_save_pointer(h, old_array);

    // Success
    return true;
}

static void hashtable_node_reference_list_free(void* elem)
{
    hashtable_node_free((hashtable_node_t) elem);
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
