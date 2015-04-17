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
#include <stdint.h>
#include <stdbool.h>

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

typedef struct {
} hashtable_s;

/* --- PUBLIC FUNCTION DEFINITIONS------------------------------------------- */

hashtable_t hashtable_create(hash_f_t hash_f)
{
}

void hashtable_free(hashtable_t h)
{
}

bool hashtable_contains(hashtable_t h, hashtable_key_t key)
{
    return false;
}

hashtable_elem_t hashtable_get(hashtable_t h, hashtable_key_t key)
{
    return NULL;
}

hashtable_elem_t hashtable_remove(hashtable_t h, hashtable_key_t key)
{
    return NULL;
}

bool hashtable_insert(hashtable_t h, hashtable_key_t key, hashtable_val_t val)
{
    return false;
}

/** @} addtogroup HASHTABLE */
