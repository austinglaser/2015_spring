/**
 * @file    reference_list.h
 * @author  Ausitin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Interface for a concurrent reference list, to store pointers
 *          for later deallocation
 */

#ifndef REFERENCE_LIST_H_
#define REFERENCE_LIST_H_

/**
 * @defgroup REFERENCE_LIST
 * @{
 */

/* --- PUBLIC DEPENDENCIES -------------------------------------------------- */

// Standard
#include <stdint.h>

/* --- PUBLIC DATA TYPES ---------------------------------------------------- */

/**
 * @brief   The reference list structure
 */
typedef struct reference_list_t_* reference_list_t;

/**
 * @brief   Function signature of a free function
 *
 * A free function takes a generic pointer, and
 * frees the memory referenced
 */
typedef void (*free_f_t)(void *);

/* --- PUBLIC FUNCTION PROTOTYPES ------------------------------------------- */

/**
 * @brief   Creates a new reference list
 *
 * @warming     This function is not thread safe. It must only be called once
 *              per object instance
 *
 * @param[in] free_f:   A function to free the list elements upon deletion. This paramater may NOT be NULL
 *
 * @return      A reference to the newly created list, or NULL if memory allocation failed
 */
reference_list_t reference_list_create(free_f_t free_f);

/**
 * @brief   Frees a reference list, and all associated memory
 *
 * This function will call free_f (@see reference_list_create) on each
 * stored reference, this being the purpose of the class
 *
 * @param[in] r:        The list to free
 */
void reference_list_free(reference_list_t r);

/**
 * @brief   Saves elem for later deallocation
 *
 * elem should be of a type which can be deallocated by free_f (@see reference_iist_create)
 *
 * @param[in] r:        Insert into this list
 * @param[in] elem:     The pointer to store
 *
 * @return      An error code
 * @retval      0:  Success
 * @retval      >0: Memory allocation failed. If this is encountered, this is NOT the right library for thie application
 */
uint32_t reference_list_insert(reference_list_t r, void * elem);

/** @} defgroup REFERENCE_LIST */

#endif //#ifndef REFERENCE_LIST_H_
