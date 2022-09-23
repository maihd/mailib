#pragma once

#include <stdint.h>
#include <Memory/Allocations.h>

template <typename T>
struct Array
{
    int32_t         count;
    int32_t         capacity;
    Allocator*      allocator;
    T				elements[];
};

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
