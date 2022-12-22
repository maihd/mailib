#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <text/string_types.h>

#ifdef __cplusplus
extern "C" {
#endif

const char*             string_ref(const char* source);
const char*             string_new(const char* source);
void                    string_free(const char* target);

const char*             string_format(int32_t bufferSize, const char* format, ...);
const char*             string_format_argv(int32_t bufferSize, const char* format, va_list argv);

const char*             string_from(void* buffer, int32_t bufferSize, const char* source);
const char*             string_format_buffer(void* buffer, int32_t bufferSize, const char* format, ...);
const char*             string_format_buffer_argv(void* buffer, int32_t bufferSize, const char* format, va_list argv);

int32_t                 string_length(const char* target);
int32_t                 string_calc_length(const char* target);

const StringBuffer*     string_get_buffer(const char* target);

bool                    string_is_heap(const char* target);
bool                    string_is_weak(const char* target);
bool                    string_is_smart(const char* target);
bool                    string_is_invalid_pointer(const char* target);

#ifdef __cplusplus
}
#endif

inline bool string_is_heap(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_HEAP;
}

inline bool string_is_weak(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_WEAK;
}

inline bool string_is_smart(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_HEAP || buffer->memtag == STRING_MEMTAG_WEAK;
}

inline bool string_is_invalid_pointer(const char* target)
{
    return (uintptr_t)target > sizeof(StringBuffer);
}

inline int32_t string_length(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    if (string_is_smart(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        return buffer->length;
    }
    else
    {
        return string_calc_length(target);
    }
}

inline const StringBuffer* string_get_buffer(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    if (string_get_buffer(target))
    {
        return (StringBuffer*)(target - sizeof(StringBuffer));
    }
    else
    {
        return nullptr;
    }
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
