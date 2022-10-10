#pragma once

#include <stdint.h>

typedef struct HashIndexor
{
    uint32_t*   hashes;
    int32_t*    indices;
    
    int32_t     hashCount;
    int32_t     indexCount;

    int32_t     hashMask;
    int32_t     lookupMask;
} HashIndexor;

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++


