#include <assert.h>
#include <stdlib.h>

#include <meta/keywords.h>

#include <memory/alloc.h>
#include <memory/global_alloc.h>

static uint8_t* global_allocator_alloc(Allocator* allocator, void* oldBlock, int32_t size, int32_t alignment)
{
    __unused(allocator);
    return memory_global_alloc(oldBlock, size, alignment);
}

static void global_allocator_free(Allocator* allocator, void* block)
{
    __unused(allocator);
    memory_global_free(block);
}

static Allocator gAllocator = {
      global_allocator_alloc,
      global_allocator_free
};

Allocator* memory_get_global_allocator(void)
{
    return &gAllocator;
}

uint8_t* memory_global_alloc(void* oldBlock, int32_t size, int32_t alignment)
{
    assert(size == 0 || (size > 0 && alignment > 0));
    return (uint8_t*)_aligned_realloc(oldBlock, size, alignment);
}

void memory_global_free(void* block)
{
    assert(block != nullptr);
    _aligned_free(block);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
