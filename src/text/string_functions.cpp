#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <memory/global_alloc.h>
#include <threading/atomic_ops.h>
#include <text/string_functions.h>

constexpr char* EMPTY_STRING = "";
constexpr int32_t STRING_ALIGNMENT = 4;

StringBuffer* string_buffer_new(int32_t length)
{
    StringBuffer* buffer = (StringBuffer*)memory_global_alloc(nullptr, length + 1 + sizeof(StringBuffer), STRING_ALIGNMENT);
    assert(buffer != NULL && "Cannot create new buffer, maybe the system have been out of memory.");

    buffer->memref      = 1;
    buffer->memtag      = STRING_MEMTAG_HEAP;
    buffer->length      = length;
    buffer->capacity    = length + 1;
    return buffer;
}

const char* string_ref(const char* source)
{
    assert(source != nullptr && "Attempt to use null-pointer on string");

    if (string_is_heap(source))
    {
        StringBuffer* buffer = (StringBuffer*)(source - sizeof(StringBuffer));
        atomic_add_i32(&buffer->memref, 1);
        return source;
    }

    return source;
}

const char* string_new(const char* source)
{
    assert(source != nullptr && "Attempt to use null-pointer on string");

    if (string_is_heap(source))
    {
        StringBuffer* buffer = (StringBuffer*)(source - sizeof(StringBuffer));
        atomic_add_i32(&buffer->memref, 1);
        return source;
    }

    const int32_t length = string_length(source);
    if (length == 0)
    {
        return EMPTY_STRING;
    }
    else
    {
        StringBuffer* buffer = string_buffer_new(length);
        strncpy(buffer->data, source, length);
        return buffer->data;
    }
}

void string_free(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    if (string_is_heap(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        
        const int32_t memref = atomic_sub_i32(&buffer->memref, 1);
        if (memref <= 0)
        {
            memory_global_free(buffer);
        }
    }   
}

const char* string_from(void* buffer, int32_t buffer_size, const char* source)
{
    assert(buffer != nullptr && "Attempt to use null-pointer on buffer");
    assert(source != nullptr && "Attempt to use null-pointer on string");

    const int32_t length = string_length(source);
    if (length == 0)
    {
        return EMPTY_STRING;
    }
    else
    {
        assert(buffer_size >= length && "Buffer is too small");
        StringBuffer* string_buffer = (StringBuffer*)buffer;
        string_buffer->length = length;
        string_buffer->memtag = STRING_MEMTAG_WEAK;

        strncpy(string_buffer->data, source, length);
        string_buffer->data[length] = '\0';
        
        return string_buffer->data;
    }
}

const char* string_format(int32_t buffer_size, const char* format, ...)
{
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = string_buffer_new(buffer_size);

    va_list argv;
    va_start(argv, format);
    int32_t length = (int32_t)vsnprintf(buffer->data, buffer_size, format, argv);
    va_end(argv);

    buffer->length = length;
    return buffer->data;
}

const char* string_format_argv(int32_t buffer_size, const char* format, va_list argv)
{
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = string_buffer_new(buffer_size + sizeof(StringBuffer));
    buffer->length = (int32_t)vsnprintf(buffer->data, buffer_size, format, argv);
    return buffer->data;
}

const char* string_format_buffer(void* buffer, int32_t buffer_size, const char* format, ...)
{
    assert(buffer != nullptr && "Attempt to use null-pointer on buffer");
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* string_buffer = (StringBuffer*)(buffer);

    va_list argv;
    va_start(argv, format);
    int32_t length = (int32_t)vsnprintf(string_buffer->data, buffer_size - sizeof(StringBuffer), format, argv);
    va_end(argv);

    string_buffer->length = length;
    string_buffer->memtag = STRING_MEMTAG_WEAK;
    
    return string_buffer->data;
}

const char* string_format_buffer_argv(void* buffer, int32_t buffer_size, const char* format, va_list argv)
{
    assert(buffer != nullptr && "Attempt to use null-pointer on buffer");
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* string_buffer = (StringBuffer*)(buffer);

    string_buffer->length = (int32_t)vsnprintf(string_buffer->data, buffer_size - sizeof(StringBuffer), format, argv);
    string_buffer->memtag = STRING_MEMTAG_WEAK;
    
    return string_buffer->data;
}

int32_t string_calc_length(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on target");
    return (int32_t)strlen(target);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
