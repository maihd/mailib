#pragma once

#include <stdint.h>
#include <assert.h>
#include <meta/keywords.h>

typedef struct Allocator Allocator;

typedef uint8_t*    AllocatorAllocFn(Allocator* allocator, void* old_block, int32_t size, int32_t alignment);
typedef void        AllocatorFreeFn(Allocator* allocator, void* block);

struct Allocator
{
    AllocatorAllocFn*       alloc_fn;
    AllocatorFreeFn*        free_fn;
};

/// Allocate a memory block
/// @param: allocator
/// @param: old_block - block you want to resize
/// @param: size - size of memory block
/// @param: alignment - Address alignment for cache-friendly, fast processing
inline uint8_t* memory_alloc(Allocator* allocator, void* old_block, int32_t size, int32_t alignment)
{
    assert(allocator != nullptr);
    assert(size > 0 || (size == 0 && alignment == 0));
    return allocator->alloc_fn(allocator, old_block, size, alignment);
}

/// Allocate a memory block
/// @param: allocator
/// @param: block
inline void memory_free(Allocator* allocator, void* block)
{
    assert(allocator != nullptr);
    assert(block != nullptr && "Bad behaviour: attempt to free nullptr");
    allocator->free_fn(allocator, block);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
