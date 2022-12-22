#pragma once

#include <stdbool.h>
#include <container/hash_indexor_types.h>

#ifdef __cplusplus
extern "C" {
#endif

void        hash_indexor_init(HashIndexor* indexor, int32_t hash_count, int32_t index_count);
void        hash_indexor_grow(HashIndexor* indexor, int32_t hash_count, int32_t index_count);
void        hash_indexor_free(HashIndexor* indexor);

bool        hash_indexor_contains_hash(const HashIndexor* indexor, uint32_t hash);
bool        hash_indexor_contains_index(const HashIndexor* indexor, uint32_t index);

int32_t     hash_indexor_get(const HashIndexor* indexor, uint32_t hash);
int32_t     hash_indexor_next(const HashIndexor* indexor, uint32_t index);

bool        hash_indexor_add(HashIndexor* indexor, uint32_t hash, int32_t index);
bool        hash_indexor_remove(HashIndexor* indexor, uint32_t hash, int32_t index);

void        hash_indexor_clear(HashIndexor* indexor);

void        hash_indexor_resize_index(HashIndexor* indexor, int32_t required_index_count);
int32_t     hash_indexor_get_spread(const HashIndexor* indexor);

#ifdef __cplusplus
}
#endif

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
