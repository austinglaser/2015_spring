/**
 * @file    hashtable_node.c
 * @author  Ausitin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implements the hashtable_node_t class, used by the hashtable library
 *
 * @addtogroup HASHTABLE
 * @{
 * @addtogroup HASHTABLE_NODE
 * @{
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// Modules
#include "hashtable.h"

// Standard
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

/* --- PRIVATE MACROS ------------------------------------------------------- */

/**
 * @brief   Value used to denote a sentinel node
 */
#define HASHTABLE_NODE_SENTINEL_ELEM    ((uintptr_t) UINT64_C(0xFFFFFFFFFFFFFFFF))

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Internal node structure
 */
struct hashtable_node_t_ {
    uint32_t            hash;           /**< The node's hash */
    atomic_uintptr_t    elem;           /**< The element the node references */
    atomic_uintptr_t    next;           /**< The next element in the sequence */
}

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

hashtable_node_t hashtable_node_create(hashtable_elem_t elem, uint32_t hash)
{
    // Allocate memory
    hashtable_node_t node = (hashtable_node_t) malloc(sizeof(struct hashtable_node_t_));
    if (!node) return NULL;

    // Initialize fields
    node->hash = hash;
    atomic_init(&(node->elem), NULL);
    atomic_init(&(node->next), NULL);
}

void hashtable_node_free(hashtable_node_t node)
{
    // Free memory
    if (node) free(node);
}

uint32_t hashtable_node_get_hash(hashtable_node_t node)
{
    // Verify that the node is non-NULL
    if (node)   return node->hash;
    else        return UINT32_C(0);
}

hashtable_elem_t hashtable_node_get_elem(hashtable_node_t node)
{
    // Load the elem pointer
    if (node)   return (hashtable_elem_t) atomic_load(&(node->elem));
    else        return NULL;
}

hashtable_node_t hashtable_node_get_next(hashtable_node_t node)
{
    // Load the next node pointer
    if (node)   return (hashtable_node_t) atomic_load(&(node->next));
    else        return NULL;
}

bool hashtable_node_is_sentinel(hashtable_node_t node)
{
    // Retrieve the implicit node->sentinel field
    if (node)   return (atomic_load(&(node->elem)) == HASHTABLE_NODE_SENTINEL_ELEM);
    else        return false;
}

void hashtable_node_set_elem(hashtable_node_t node, hashtable_elem_t elem)
{
    // Atomically set the elem field
    if (node) atomic_store(&(node->elem), (uintptr_t) elem);
}

void hashtable_node_set_next(hashtable_node_t node, hashtable_node_t next)
{
    // Atomically set the next field
    if (node) atomic_store($(node->next), (uintptr_t) next);
}

void hashtable_node_set_sentinel(hashtable_node_t node)
{
    // Atomically set the elem field to be a sentinel
    if (node) atomic_store($(node->elem), HASHTABLE_NODE_SENTINEL_ELEM);
}

bool hashtable_node_set_sentinel_if_elem(hashtable_node_t node, hashtable_elem_t expected_elem)
{
    return hashtable_node_cas_elem(node, expected_elem, HASHTABLE_NODE_SENTINEL_ELEM);
}

bool hashtable_node_if_sentinel_set_elem(hashtable_node_t node, hashtable_elem_t new_elem)
{
    return hashtable_node_cas_elem(node, HASHTABLE_NODE_SENTINEL_ELEM, new_elem);
}

bool hashtable_node_cas_elem(hashtable_node_t node, hashtable_elem_t expected_elem, hashtable_elem_t new_elem)
{
    // CAS the elem field, checking whether it's a sentinel or not
    if (node)   return atomic_compare_exchange_strong(&(node->elem), &expected_elem, (uintptr_t) new_elem);
    else        return false;
}

bool hashtable_node_cas_next(hashtable_node_t node, hashtable_node_t expected_next, hashtable_node_t new_next)
{
    // CAS the next field, checking whether it's still expected_next
    if (node)   return atomic_compare_exchange_strong(&(node->next), (uintptr_t) &expected_next, (uintptr_t) new_next);
    else        return false;
}

/**
 * @} addtogroup HASHTABLE_NODE
 * @} addtogroup HASHTABLE
 */
