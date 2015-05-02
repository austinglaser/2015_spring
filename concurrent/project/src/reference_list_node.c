/**
 * @file    reference_list.c
 * @author  Ausitin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implementation of a node type for reference lists
 *
 * @addtogroup REFERENCE_LIST_NODE
 * @{
 */

/* --- PUBLIC DEPENDENCIES -------------------------------------------------- */

// This module
#include "reference_list_node.h"

// Standard
#include <stdbool.h>
#include <stdlib.h>

/* --- PUBLIC DATA TYPES ---------------------------------------------------- */

/**
 * @brief   Internal data type for a reference list node
 */
struct reference_list_node_t_ {
};

/* --- PUBLIC FUNCTION PROTOTYPES ------------------------------------------- */

reference_list_node_t reference_list_node_create(void* ref)
{
    return NULL;
}

void* reference_list_node_get_ref(reference_list_node_t node)
{
    return NULL;
}

reference_list_node_t reference_list_node_get_next(reference_list_node_t node)
{
    return NULL;
}

bool reference_list_node_set_next(reference_list_node_t node, reference_list_node_t next)
{
    return false;
}

/** @} addtogroup REFERENCE_LIST_NODE */
