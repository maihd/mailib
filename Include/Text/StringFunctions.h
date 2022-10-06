#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <Text/StringTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

const char*             String_New(const char* source);
void                    String_Free(const char* target);

const char*             String_Format(int32_t bufferSize, const char* format, ...);
const char*             String_FormatArgv(int32_t bufferSize, const char* format, va_list argv);

const char*             String_From(void* buffer, int32_t bufferSize, const char* source);
const char*             String_FormatBuffer(void* buffer, int32_t bufferSize, const char* format, ...);
const char*             String_FormatBufferArgv(void* buffer, int32_t bufferSize, const char* format, va_list argv);

int32_t                 String_Length(const char* target);
const StringBuffer*     String_GetBuffer(const char* target);

bool                    String_IsHeap(const char* target);
bool                    String_IsWeak(const char* target);
bool                    String_IsSmart(const char* target);
bool                    String_IsInvalidPointer(const char* target);

#ifdef __cplusplus
}
#endif

inline bool String_IsHeap(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_HEAP;
}

inline bool String_IsWeak(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_WEAK;
}

inline bool String_IsSmart(const char* target)
{
    assert(target != nullptr && "Attempt to use null-pointer on string");

    StringBuffer* buffer = (StringBuffer*)(target - sizeof(StringBuffer));
    return buffer->memtag == STRING_MEMTAG_HEAP || buffer->memtag == STRING_MEMTAG_WEAK;
}

inline bool String_IsInvalidPointer(const char* target)
{
    return (uintptr_t)target > sizeof(StringBuffer);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
