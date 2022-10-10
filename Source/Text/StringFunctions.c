#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <Threading/AtomicOps.h>
#include <Text/StringFunctions.h>
#include <Memory/GeneralAllocations.h>

constexpr char* EMPTY_STRING = "";
constexpr int32_t STRING_ALIGNMENT = 4;

StringBuffer* StringBuffer_New(int32_t length)
{
    StringBuffer* buffer = (StringBuffer*)Memory_GeneralAllocate(nullptr, length + 1 + sizeof(StringBuffer), STRING_ALIGNMENT);
    assert(buffer != NULL && "Cannot create new buffer, maybe the system have been out of memory.");

    buffer->length = length;
    buffer->capacity = length + 1;

    buffer->memref = 1;
    buffer->memtag = STRING_MEMTAG_HEAP;
    return buffer;
}

const char* String_New(const char* source)
{
    assert(source != nullptr && "Attempt to use null-pointer on string");

    if (String_IsHeap(source))
    {
        StringBuffer* buffer = (StringBuffer*)(source - sizeof(StringBuffer));
        Atomic_AddI32(buffer->memref, 1);
        return source;
    }

    int32_t length = String_Length(source);
    if (length == 0)
    {
        return EMPTY_STRING;
    }
    else
    {
        StringBuffer* buffer = StringBuffer_New(length);
        
        strncpy(buffer->data, source, length);
        
        return buffer->data;
    }
}

void String_Free(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    if (String_IsHeap(target))
    {
        StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
        
        int memref = Atomic_SubI32(buffer->memref, 1);
        if (memref <= 0)
        {
            Memory_GeneralFree(buffer);
        }
    }   
}

const char* String_From(void* buffer, int32_t bufferSize, const char* source)
{
    assert(buffer != nullptr && "Attempt to use null-pointer on buffer");
    assert(source != nullptr && "Attempt to use null-pointer on string");

    int32_t length = String_Length(source);
    if (length == 0)
    {
        return EMPTY_STRING;
    }
    else
    {
        assert(bufferSize >= length && "Buffer is too small");
        StringBuffer* stringBuffer = (StringBuffer*)buffer;
        stringBuffer->length = length;
        stringBuffer->memtag = STRING_MEMTAG_WEAK;

        strncpy(stringBuffer->data, source, length);
        stringBuffer->data[length] = '\0';
        
        return stringBuffer->data;
    }
}

const char* String_Format(int32_t bufferSize, const char* format, ...)
{
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = StringBuffer_New(bufferSize);

    va_list argv;
    va_start(argv, format);
    int32_t length = (int32_t)vsnprintf(buffer->data, bufferSize, format, argv);
    va_end(argv);

    buffer->length = length;
    return buffer->data;
}

const char* String_FormatArgv(int32_t bufferSize, const char* format, va_list argv)
{
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = StringBuffer_New(bufferSize + sizeof(StringBuffer));
    buffer->length = (int32_t)vsnprintf(buffer->data, bufferSize, format, argv);
    return buffer->data;
}

const char* String_FormatBuffer(void* buffer, int32_t bufferSize, const char* format, ...)
{
    assert(buffer != nullptr && "Attempt to use null-pointer on buffer");
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    va_list argv;
    va_start(argv, format);
    int32_t length = (int32_t)vsnprintf(stringBuffer->data, bufferSize - sizeof(StringBuffer), format, argv);
    va_end(argv);

    stringBuffer->length = length;
    stringBuffer->memtag = STRING_MEMTAG_WEAK;
    
    return stringBuffer->data;
}

const char* String_FormatBufferArgv(void* buffer, int32_t bufferSize, const char* format, va_list argv)
{
    assert(buffer != nullptr && "Attempt to use null-pointer on buffer");
    assert(format != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* stringBuffer = (StringBuffer*)(buffer);

    stringBuffer->length = (int32_t)vsnprintf(stringBuffer->data, bufferSize - sizeof(StringBuffer), format, argv);
    stringBuffer->memtag = STRING_MEMTAG_WEAK;
    
    return stringBuffer->data;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
