#pragma once

#include <stdint.h>
#include <assert.h>
#include <Meta/Keywords.h>

typedef struct Allocator Allocator;

typedef uint8_t*    AllocatorAllocateFn(Allocator* allocator, void* oldBlock, int32_t size, int32_t alignment);
typedef void        AllocatorFreeFn(Allocator* allocator, void* block);

struct Allocator
{
    AllocatorAllocateFn*    allocateFn;
    AllocatorFreeFn*        freeFn;
};

/// Allocate a memory block
/// @param: allocator
/// @param: oldBlock - block you want to resize
/// @param: size - size of memory block
/// @param: alignment - Address alignment for cache-friendly, fast processing
inline uint8_t* Memory_Allocate(Allocator* allocator, void* oldBlock, int32_t size, int32_t alignment)
{
    assert(allocator != nullptr);
    assert(size > 0 || (size == 0 && alignment == 0));
    return allocator->allocateFn(allocator, oldBlock, size, alignment);
}

/// Allocate a memory block
/// @param: allocator
/// @param: block
inline void Memory_Free(Allocator* allocator, void* block)
{
    assert(allocator != nullptr);
    assert(block != nullptr && "Bad behaviour: attempt to free nullptr");
    allocator->freeFn(allocator, block);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
