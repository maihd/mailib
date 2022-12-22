#include <misc/testing.h>
#include <memory/global_alloc.h>
#include <container/array_functions.h>

TEST_CASE("Basic array")
{
	Allocator* allocator = memory_get_global_allocator();

	Array<int>* array = array_new<int>(allocator, 32);

	array_free(array);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
