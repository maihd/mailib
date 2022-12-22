#pragma once

#include <stdint.h>
#include <meta/platforms.h>

// Check support platform
#ifndef PLATFORM_HOSTED
#error Target platform does not support general memory allocations
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct Allocator*   memory_get_global_allocator(void);
uint8_t*            memory_global_alloc(void* old_block, int32_t size, int32_t alignment);
void                memory_global_free(void* block);

struct Allocator*   memory_get_page_allocator(void);
uint8_t*            memory_page_alloc(void* old_block, int32_t size, int32_t alignment);
void                memory_page_free(void* block);

#ifdef __cplusplus
}
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
