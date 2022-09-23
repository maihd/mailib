#include <assert.h>
#include <stdlib.h>

#include <Meta/Keywords.h>
#include <Memory/Allocations.h>
#include <Memory/GeneralAllocations.h>

static uint8_t* GeneralAllocator_Allocate(Allocator* allocator, void* oldBlock, int32_t size, int32_t alignment)
{
    __unused(allocator);
    return Memory_GeneralAllocate(oldBlock, size, alignment);
}

static void GeneralAllocator_Free(Allocator* allocator, void* block)
{
    __unused(allocator);
    return Memory_GeneralFree(block);
}

static Allocator gAllocator = {
      GeneralAllocator_Allocate,
      GeneralAllocator_Free
};

Allocator* Memory_GetGeneralAllocator(void)
{
    return &gAllocator;
}

uint8_t* Memory_GeneralAllocate(void* oldBlock, int32_t size, int32_t alignment)
{
    assert(size == 0 || (size > 0 && alignment > 0));
    return (uint8_t*)_aligned_realloc(oldBlock, size, alignment);
}

void Memory_GeneralFree(void* block)
{
    assert(block != nullptr);
    _aligned_free(block);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
