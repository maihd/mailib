#include <Misc/Testing.h>
#include <Container/ArrayFunctions.h>
#include <Memory/GeneralAllocations.h>

TEST_CASE("Basic array")
{
	Allocator* allocator = Memory_GetGeneralAllocator();

	Array<int>* array = Array_New<int>(allocator, 32);

	Array_Free(array);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
