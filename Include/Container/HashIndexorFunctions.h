#pragma once

#include <stdbool.h>
#include <Container/HashIndexor.h>

#ifdef __cplusplus
extern "C" {
#endif

void        HashIndexor_Init(HashIndexor* indexor, int32_t hashCount, int32_t indexCount);
void        HashIndexor_Grow(HashIndexor* indexor, int32_t hashCount, int32_t indexCount);
void        HashIndexor_Free(HashIndexor* indexor);

bool        HashIndexor_ContainsHash(const HashIndexor* indexor, uint32_t hash);
bool        HashIndexor_ContainsIndex(const HashIndexor* indexor, uint32_t index);

int32_t     HashIndexor_Get(const HashIndexor* indexor, uint32_t hash);
int32_t     HashIndexor_Next(const HashIndexor* indexor, uint32_t index);

bool        HashIndexor_Add(HashIndexor* indexor, uint32_t hash, int32_t index);
bool        HashIndexor_Remove(HashIndexor* indexor, uint32_t hash, int32_t index);

void        HashIndexor_Clear(HashIndexor* indexor);

void        HashIndexor_ResizeIndex(HashIndexor* indexor, int32_t requiredIndexCount);
int32_t     HashIndexor_GetSpread(const HashIndexor* indexor);

#ifdef __cplusplus
}
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
