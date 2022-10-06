#pragma once

#include <stdint.h>
#include <Meta/Keywords.h>

constexpr uint64_t STRING_MEMTAG_HEAP = 0xa020b127788efe8fULL;
constexpr uint64_t STRING_MEMTAG_WEAK = 0xb64c61277893498fULL;

typedef struct StringRef
{
    int32_t     length;
    const char* buffer;
} StringRef;

typedef struct String
{
    int32_t     length;
    char*       buffer;
} String;

typedef struct StringBuffer
{
    uint64_t    memtag;
    int32_t     memref;
    int32_t     length;
    char        data[];
} StringBuffer;

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
