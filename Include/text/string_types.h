#pragma once

#include <stdint.h>
#include <meta/keywords.h>

constexpr uint32_t STRING_MEMTAG_HEAP = 0xac8b19d1U;
constexpr uint32_t STRING_MEMTAG_WEAK = 0x2e9497f4U;

/// StringBuffer
/// @note: make sure StringBuffer is 16bytes aligned
#pragma warning(disable : 4200)
typedef struct StringBuffer
{
    uint32_t    memtag;
    int32_t     memref;

    int32_t     length;
    int32_t     capacity;

    char        data[];
} StringBuffer;

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
