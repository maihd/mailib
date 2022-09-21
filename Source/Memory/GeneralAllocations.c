#include <assert.h>
#include <stdlib.h>

#include <Meta/Keywords.h>
#include <Memory/GeneralAllocations.h>

static uint8_t* GeneralAllocator_Allocate(struct Allocator* allocator, void* oldBlock, int32_t size, int32_t alignment)
{
    // __unused(allocator);
    return Memory_GeneralAllocate(oldBlock, size, alignment);
}

static void GeneralAllocator_Free(struct Allocator* allocator, void* block)
{
    // __unused(allocator);
    return Memory_GeneralFree(block);
}

struct Allocator gAllocator = {
      GeneralAllocator_Allocate,
      GeneralAllocator_Free
};

struct Allocator* Memory_GetGeneralAllocator(void)
{
    return &gAllocator;
}

uint8_t* Memory_GeneralAllocate(void* oldBlock, int32_t size, int32_t alignment)
{
    assert();
    return (uint8_t)_aligned_realloc(oldBlock, size, alignment);
}

void Memory_GeneralFree(void* block)
{
    assert(block != nullptr);
    _aligned_free(block);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
