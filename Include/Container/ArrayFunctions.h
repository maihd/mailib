#pragma once

#include <assert.h>
#include <Memory/Allocations.h>
#include <Container/ArrayTypes.h>

template <typename T>
inline Array<T>* Array_New(Allocator* allocator, int32_t capacity)
{
    Array<T>* array = (Array<T>*)Memory_Allocate(allocator, nullptr, sizeof(Array<T>) + capacity * sizeof(T), alignof(T));
	if (array != nullptr)
	{
		array->count		= 0;
		array->capacity		= capacity;
		array->allocator	= allocator;
	}

    return array;
}

template <typename T>
inline void Array_Free(Array<T>* array)
{
    if (array != nullptr)
    {
        Memory_Free(array->allocator, array);
    }
}

template <typename T>
inline bool Array_EnsureSize(const Array<T>* array, int32_t size)
{
    return array.capacity >= size;
}

template <typename T>
inline void Array_Push(Array<T>* array, const T& value)
{
    assert(array != nullptr);
    assert(Array_EnsureSize(array, array.count + 1));
    array.elements[array.count++] = value;
}

template <typename T>
inline T Array_Pop(Array<T>* array)
{
    assert(array != nullptr);
    assert(array.count > 0 && "Array is empty");
    return array.elements[--array.count];
}

template <typename T>
inline void Array_Clear(Array<T>* array)
{
    assert(array != nullptr);
    array->count = 0;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
