#include <Misc/Testing.h>
#include <Memory/GeneralAllocations.h>
#include <Container/HashTableFunctions.h>

TEST_CASE("Basic HashTable")
{
	Allocator* allocator = Memory_GetGeneralAllocator();

	HashTable<int32_t>* array = HashTable_New<int32_t>(allocator);

	HashTable_Free(array);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
