/**
 * @file    reference_list.c
 * @author  Ausitin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implementation of a node type for reference lists
 *
 * @addtogroup REFERENCE_LIST_NODE
 * @{
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// This module
#include "reference_list_node.h"

// Standard
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ATOMIC_NULL         ((atomic_uintptr_t) 0)

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Internal data type for a reference list node
 */
struct reference_list_node_t_ {
    void*               ref;    /**< The stored reference */
    atomic_uintptr_t    next;   /**< The next node in the list */
};

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

reference_list_node_t reference_list_node_create(void* ref)
{
    reference_list_node_t node;

    // Allocate memory
    node = (reference_list_node_t) malloc(sizeof(struct reference_list_node_t_));
    if (!node) return NULL;

    // Initialize fields
    node->ref = ref;
    atomic_init(&(node->next), ATOMIC_NULL);

    // Pass it back
    return node;
}

void reference_list_node_free(reference_list_node_t node) {
    // Free the memory
    if (node) free(node);
}

void* reference_list_node_get_ref(reference_list_node_t node)
{
    // Check input
    if (!node) return NULL;

    // Return reference
    return node->ref;
}

reference_list_node_t reference_list_node_get_next(reference_list_node_t node)
{
    return (reference_list_node_t) atomic_load(&(node->next));
}

bool reference_list_node_set_next(reference_list_node_t node, reference_list_node_t next)
{
    // Check input
    if (!node) return false;
    if (!next) return false;

    // Attempt to set next
    atomic_uintptr_t expected = ATOMIC_NULL;
    return atomic_compare_exchange_strong(&(node->next), &expected, (atomic_uintptr_t) next);
}

/** @} addtogroup REFERENCE_LIST_NODE */
