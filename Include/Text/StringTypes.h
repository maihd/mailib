#pragma once

#include <stdint.h>

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

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
