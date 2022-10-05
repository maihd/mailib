#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Memory/Allocations.h>
#include <FileSystem/FileSystem.h>

// -------------------------------------------------------------
// FileInterface functions prototype
// -------------------------------------------------------------

static bool     MemoryStream_Open(FileInterface* inteface, void* buffer, int32_t bufferSizeInBytes, FileStream* outStream);
static bool     MemoryStream_Close(FileStream* inteface);

static int32_t  MemoryStream_Read(FileStream* stream, void* outputBuffer, int32_t bufferSizeInBytes);
static int32_t  MemoryStream_Write(FileStream* stream, const void* inputBuffer, int32_t bufferSizeInBytes);

static bool     MemoryStream_Seek(FileStream* stream, SeekOffset offset, int32_t count);
static int32_t  MemoryStream_GetSize(const FileStream* stream);
static int32_t  MemoryStream_GetCursor(const FileStream* stream);

static bool     MemoryStream_Flush(FileStream* stream);
static bool     MemoryStream_IsAtEnd(const FileStream* stream);

// -------------------------------------------------------------
// FileInterfaces
// -------------------------------------------------------------

static FileInterface gMemoryFileSystem = {
    MemoryStream_Open,
    MemoryStream_Close,

    MemoryStream_Read,
    MemoryStream_Write,

    MemoryStream_Seek,
    MemoryStream_GetSize,
    MemoryStream_GetCursor,

    MemoryStream_Flush,
    MemoryStream_IsAtEnd
};

// -------------------------------------------------------------
// FileStream functions
// -------------------------------------------------------------

bool FileStream_FromMemory(FileStream* stream, void* buffer, int32_t bufferSize)
{
	return MemoryStream_Open(&gMemoryFileSystem, buffer, bufferSize, stream);
}

// -------------------------------------------------------------
// FileInterface functions define
// -------------------------------------------------------------

bool MemoryStream_Open(FileInterface* inteface, void* inputBuffer, int32_t bufferSizeInBytes, FileStream* outStream)
{
    FileStream stream;
    stream.inteface      = inteface;
    stream.memory.buffer = inputBuffer;
    stream.memory.length = bufferSizeInBytes;
    stream.memory.cursor = 0;
    stream.memory.flags  = MemoryStreamFlags_None;

    *outStream = stream;
    return true;
}

bool MemoryStream_Close(FileStream* stream)
{
    if (stream && stream->inteface == &gMemoryFileSystem)
    {
        if (stream->memory.flags & MemoryStreamFlags_BufferOwner)
        {
            //Memory_Free(stream->memory.buffer);
        }

        return true;
    }

    return false;
}

int32_t MemoryStream_Read(FileStream* stream, void* outputBuffer, int32_t bufferSizeInBytes)
{
    const int32_t readBytes = stream->memory.cursor + bufferSizeInBytes > stream->memory.length 
        ? stream->memory.length - stream->memory.cursor
        : bufferSizeInBytes;

    memcpy(outputBuffer, (uint8_t*)stream->memory.buffer + stream->memory.cursor, readBytes);
    stream->memory.cursor += readBytes;
    return readBytes;
}

int32_t MemoryStream_Write(FileStream* stream, const void* inputBuffer, int32_t bufferSizeInBytes)
{
    const int32_t writeBytes = stream->memory.cursor + bufferSizeInBytes > stream->memory.length
        ? stream->memory.length - stream->memory.cursor
        : bufferSizeInBytes;

    memcpy((uint8_t*)stream->memory.buffer + stream->memory.cursor, inputBuffer, writeBytes);
    stream->memory.cursor += writeBytes;
    return writeBytes;
}

bool MemoryStream_Seek(FileStream* stream, SeekOffset offset, int32_t count)
{
    int32_t cursor;
    switch (offset)
    {
    case SeekOffset_CurrentPosition:
        cursor = stream->memory.cursor + count;
        break;

    case SeekOffset_EndOfFile:
        cursor = stream->memory.length - count;
        break;

    case SeekOffset_StartOfFile:
        cursor = count;
        break;

    default:
        assert(false && "Invalid seek offset");
        cursor = -1;
        break;
    }

    if (cursor < 0 && cursor >= stream->memory.length)
    {
        return false;
    }

    stream->memory.cursor = cursor;
    return true;
}

int32_t MemoryStream_GetSize(const FileStream* stream)
{
    return stream->memory.length;
}

int32_t MemoryStream_GetCursor(const FileStream* stream)
{
    return stream->memory.cursor;
}

bool MemoryStream_Flush(FileStream* stream)
{
    return true;
}

bool MemoryStream_IsAtEnd(const FileStream* stream)
{
    return stream->memory.cursor == stream->memory.length;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
