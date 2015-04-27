/**
 * @file    hashtable_node.h
 * @author  Ausitin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Implements the hashtable_node_t class, used by the hashtable library
 */

#ifndef HASHTABLE_NODE_H_
#define HASHTABLE_NODE_H_

 /**
 * @addtogroup HASHTABLE
 * @{
 * @addtogroup HASHTABLE_NODE
 * @{
 */

/* --- PUBLIC DEPENDENCIES -------------------------------------------------- */

// Modules
#include "hashtable.h"

// Standard
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

/* --- PUBLIC DATA TYPES ---------------------------------------------------- */

/**
 * @brief   A reference to a hashtable node
 */
typedef struct hashtable_node_t_ * hashtable_node_t;

/* --- PUBLIC FUNCTION PROTOTYPES ------------------------------------------- */

/**
 * @brief   Allocates and returns a new hashtable node structure
 *
 * The new node's sentinel and next values will be false and NULL respectively --
 * to set them, call the appropriate hashtable_node_set functions
 *
 * @param[in] elem:             The element for the structure
 * @param[in] hash:             The associated key hash
 *
 * @return:     An allocated hashtable node, or NULL if memory allocation failed
 */
hashtable_node_t hashtable_node_create(hashtable_elem_t elem, uint32_t hash);

/**
 * @brief   De-allocates memory associated with a hashtable node
 *
 * @param[in] node:             The node to be freed
 */
void hashtable_node_free(hashtable_node_t node);

/**
 * @brief   Gets the hash value from a hashtable node
 *
 * @param[in] node:             The node to extract information from
 *
 * @return      The 32 bit hash, or 0 if getting failed
 */
uint32_t hashtable_node_get_hash(hashtable_node_t node);

/**
 * @brief   Gets the element stored in a hashtable node
 *
 * @param[in] node:             The node to extract information from
 * 
 * @return      The element stored in the node, or NULL if retrieval failed
 */
hashtable_elem_t hashtable_node_get_elem(hashtable_node_t node);

/**
 * @brief   Gets the next node in the node list, or NULL if there is no next
 *
 * @param[in] node:             The node to extract the predecessor from
 *
 * @return      The node's predecessor, or NULL if there is none
 */
hashtable_node_t hashtable_node_get_next(hashtable_node_t node);

/**
 * @brief   Determines whether node is a sentinel or not
 *
 * @param[in] node:             The node to extradct information from
 *
 * @return      true if the node is a sentinel, false otherwise
 */
bool hashtable_node_is_sentinel(hashtable_node_t node);

/**
 * @brief   Sets node's element to elem
 *
 * @note    If node was a sentinel, it will not be considered so after a call
 *          to this function
 *
 * @param[in] node:             The node to modify
 * @param[in] elem:             The element to insert
 */
void hashtable_node_set_elem(hashtable_node_t node, hashtable_elem_t elem);

/**
 * @brief   Sets the predecessor of node to next
 *
 * @param[in,out] node:         The node to modify
 * @param[in] next:             node's new predecessor
 */
void hashtable_node_set_next(hashtable_node_t node, hashtable_node_t next);

/**
 * @brief   Makes node into a sentinel if it wasn't already
 *
 * @note    node's elem will read as NULL after a call to this function
 *
 * @param[in, out] node:        The node to modify
 *
 * @return      true if node was made into a sentinel, false if it already was one
 */
bool hashtable_node_set_sentinel(hashtable_node_t node);

/**
 * @brief   Atomically checks whether node is a sentinel, and if so sets the elem to new_elem
 *
 * @param[in,out] node:         The node to modify
 * @param[in] new_elem:         The element to try to set
 *
 * @return      true if node was a sentinel and new_elem was successfully set, false otherwise
 */
bool hashtable_node_if_sentinel_set_elem(hashtable_node_t node, hashtable_elem_t new_elem);

/**
 * @brief   Atomically compares node's next value to expected next, sets it to new_hash if they are equal
 *
 * @paran[in,out] node:         The node to modify
 * @param[in] expected_next:    The node we expect to be the predecessor
 * @param[in] new_next:         The predecessor we're attempting to set
 *
 * @return      true if the CAS succeeded, false if node remains unchanged
 */
bool hashtable_node_cas_next(hashtable_node_t node, hashtable_node_t expected_next, hashtable_node_t new_next);

/**
 * @} addtogroup HASHTABLE_NODE
 * @} addtogroup HASHTABLE
 */
#endif  //#ifndef HASHTABLE_NODE_H_
