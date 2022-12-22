#include <misc/testing.h>
#include <memory/global_alloc.h>
#include <container/hash_table_functions.h>

TEST_CASE("Basic HashTable")
{
	Allocator* allocator = memory_get_global_allocator();

	HashTable<int32_t>* array = hash_table_new<int32_t>(allocator);

	hash_table_free(array);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
