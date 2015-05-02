/**
 * @file    reference_list.h
 * @author  Ausitin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implementation of a node type for reference lists
 */

#ifndef REFERENCE_LIST_NODE_H_
#define REFERENCE_LIST_NODE_H_

/**
 * @defgroup REFERENCE_LIST_NODE
 * @{
 */

/* --- PUBLIC DEPENDENCIES -------------------------------------------------- */

// Standard
#include <stdbool.h>
#include <stdlib.h>

/* --- PUBLIC DATA TYPES ---------------------------------------------------- */

/**
 * @brief   Data type for a reference list node
 */
typedef struct reference_list_node_t_* reference_list_node_t;

/* --- PUBLIC FUNCTION PROTOTYPES ------------------------------------------- */

/**
 * @brief       Creates a new reference list node
 *
 * @param[in] ref:      The reference to store
 *
 * @return      A reference to the new node, or NULL if memory allocation failed
 */
reference_list_node_t reference_list_node_create(void* ref);

/**
 * @brief       Gets the next node referenced
 *
 * @param[in] node:     The node to get the next value from
 *
 * @return      A reference to the next node
 */
reference_list_node_t reference_list_node_get_next(reference_list_node_t node);

/**
 * @brief       Tries to set the next field of node to next
 *
 * This function will only succeed if node->next is NULL. Otherwise, 
 * the next field is no longer modifiable
 *
 * @param[in,out] node: The node we're trying to modify
 * @param[in] next:     The desired next node in the list
 *
 * @return      true if the assignment was successful, false otherwise
 */
bool reference_list_node_set_next(reference_list_node_t node, reference_list_node_t next);

/** @} defgroup REFERENCE_LIST_NODE */

#endif //#ifndef REFERENCE_LIST_NODE_H_
