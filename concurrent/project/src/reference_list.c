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

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   The reference list structure
 */
struct reference_list_t_ {
};

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

reference_list_t reference_list_create(free_f_t free_f)
{
    return NULL;
}

void reference_list_free(reference_list_t r)
{
}

uint32_t reference_list_insert(reference_list_t r, void * elem)
{
    return 1;
}

/** @} addgtogroup REFERENCE_LIST */

