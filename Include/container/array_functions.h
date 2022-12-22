#pragma once

#include <assert.h>
#include <string.h>
#include <memory/alloc.h>
#include <container/array_types.h>

template <typename T>
inline Array<T>* array_new(Allocator* allocator, int32_t capacity)
{
    Array<T>* array = (Array<T>*)memory_alloc(allocator, nullptr, sizeof(Array<T>) + capacity * sizeof(T), alignof(T));
	if (array != nullptr)
	{
		array->count		= 0;
		array->capacity		= capacity;
		array->allocator	= allocator;
	}

    return array;
}

template <typename T>
inline void array_free(Array<T>* array)
{
    if (array != nullptr)
    {
        memory_free(array->allocator, array);
    }
}

template <typename T>
inline bool array_ensure_size(const Array<T>* array, int32_t size)
{
    return array->capacity >= size;
}

template <typename T>
inline Array<T>* array_ensure_size_or_new(Array<T>* array, int32_t size)
{
    if (array->capacity >= size)
    {
        return array;
    }
    else
    {
        int32_t   new_capacity  = size;
        Array<T>* new_array     = array_new<T>(array->allocator, new_capacity);
        
        if (new_array != nullptr)
        {
            new_array->count = array->count;
            memcpy(new_array->elements, array->elements, array->count * sizeof(T));
            array_free(array);

            return newArray;
        }
        else
        {
            return array;
        }
    }
}

template <typename T>
inline void array_push(Array<T>* array, const T& value)
{
    assert(array != nullptr);
    assert(array_ensure_size(array, array.count + 1));
    array->elements[array.count++] = value;
}

template <typename T>
inline Array<T>* array_push_expand(Array<T>* array, const T& value)
{
    assert(array != nullptr);
    Array<T>* new_array = array_ensure_size_or_new(array, array->count + 1);
    array_push(new_array, value);
    return new_array;
}

template <typename T>
inline T array_pop(Array<T>* array)
{
    assert(array != nullptr);
    assert(array->count > 0 && "Array is empty");
    return array->elements[--array.count];
}

template <typename T>
inline void array_clear(Array<T>* array)
{
    assert(array != nullptr);
    array->count = 0;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
