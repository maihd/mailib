#pragma once

#include <stdint.h>

typedef struct HashIndexor
{
    uint32_t*   hashes;
    int32_t*    indices;
    
    int32_t     hash_count;
    int32_t     index_count;

    int32_t     hashMask;
    int32_t     lookupMask;
} HashIndexor;

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++


