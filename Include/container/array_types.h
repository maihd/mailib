#pragma once

#include <stdint.h>
#include <memory/alloc.h>

#pragma warning(disable : 4200)
template <typename T>
struct Array
{
    int32_t         capacity;
    Allocator*      allocator;

    int32_t         count;
    T				items[];
};

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
