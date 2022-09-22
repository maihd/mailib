#pragma once

#include <stdint.h>
#include <Meta/Platforms.h>

// Check support platform
#ifndef PLATFORM_HOSTED
#error Target platform does not support general memory allocations
#endif

struct Allocator*   Memory_GetGeneralAllocator(void);
uint8_t*            Memory_GeneralAllocate(void* oldBlock, int32_t size, int32_t alignment);
void                Memory_GeneralFree(void* block);

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
