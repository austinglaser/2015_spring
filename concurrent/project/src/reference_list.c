/**
 * @file    reference_list.c
 * @author  Ausitin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implementation of a concurrent reference list, to store pointers
 *          for later deallocation
 *
 * @addtogroup REFERENCE_LIST
 * @{
 */

/* --- PRIVATE DEPENDENCIES ------------------------------------------------- */

// This module
#include "reference_list.h"

// Standard
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

// Modules
#include "reference_list_node.h"

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   The reference list structure
 */
struct reference_list_t_ {
    reference_list_node_t   head;       /**< The beginning of the actual list */
    free_f_t                free_f;     /**< A function to free individual elements */
};

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

reference_list_t reference_list_create(free_f_t free_f)
{
    reference_list_t r;

    // Allocate memory
    r = (reference_list_t) malloc(sizeof(struct reference_list_t_));
    if (!r) return NULL;

    // Set fields
    r->free_f = free_f;
    r->head = reference_list_node_create(NULL); // The list is headed by a dummy node

    // Pass it back
    return r;
}

void reference_list_free(reference_list_t r)
{
    if (r) {
        // Free all the elements
        reference_list_node_t curr = reference_list_node_get_next(r->head);
        reference_list_node_t next;
        while(curr) {
            // Get next
            next = reference_list_node_get_next(curr);

            // Free reference and node
            r->free_f(reference_list_node_get_ref(curr));
            reference_list_node_free(curr);

            // Keep walking
            curr = next;
        }
        
        // Free the head node. No reference freeing necessary
        // because it is a dummy
        free(r->head);

        // Free the list structure
        free(r);
    }
}

uint32_t reference_list_insert(reference_list_t r, void * elem)
{
    if (!r) return 1;

    // Create a node for this element
    reference_list_node_t node = reference_list_node_create(elem);
    if (!node) return 1;

    // Loop till we successfuly put it in
    bool insert_success = false;
    do {
        // Find the end
        reference_list_node_t curr = r->head;
        reference_list_node_t next = reference_list_node_get_next(curr);
        while (next) {
            curr = next;
            next = reference_list_node_get_next(curr);
        }

        // Try to insert
        insert_success = reference_list_node_set_next(curr, node);
    } while (!insert_success);

    // Success
    return 0;
}

/** @} addgtogroup REFERENCE_LIST */

