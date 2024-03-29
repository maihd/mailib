#pragma once

#include <stdint.h>
#include <memory/alloc.h>

template <typename T>
struct HashTable
{
    int32_t     count;
    int32_t     capacity;
                
    int32_t*    hashs;
    int32_t     hash_count;
                
    int32_t*    nexts;
    uint32_t*   keys;
    T*          values;

	Allocator*  allocator;
};

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
