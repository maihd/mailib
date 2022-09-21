#pragma once

#include <stdint.h>
#include <assert.h>
#include <Meta/Keywords.h>

typedef struct Allocator Allocator;

typedef uint8_t*    AllocatorAllocateFn(Allocator* allocator, void* oldBlock, int32_t size, int32_t alignment);
typedef void        AllocatorFreeFn(Allocator* allocator, void* block);

struct Allocator
{
    AllocatorAllocateFn     allocateFn;
    AllocatorFreeFn         freeFn;
};

inline uint8_t* Memory_Allocate(Allocator* allocator, void* oldBlock, int32_t size, int32_t alignment)
{
    assert(allocator != nullptr);
    assert(size > 0 || (size == 0 && alignment == 0));
    return allocator->allocateFn(allocator, oldBlock, size, alignment);
}

inline void Memory_Free(Allocator* allocator, void* block)
{
    assert(allocator != nullptr);
    assert(block != nullptr && "Bad behaviour: attempt to free nullptr");
    return allocator->free(allocator, oldBlock, size, aligment);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
