#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Memory/Allocations.h>
#include <FileSystem/FileSystem.h>

enum
{
    SEARCH_PATH_MAX_COUNT   = 16,
    SEARCH_PATH_MAX_LENGTH  = 1024,
};

static int32_t  gSearchPathCount;
static char     gSearchPaths[SEARCH_PATH_MAX_COUNT][SEARCH_PATH_MAX_LENGTH];

static int32_t IndexOfSearchPath(const char* path)
{
    for (int32_t i = 0; i < gSearchPathCount; i++)
    {
        if (strcmp(gSearchPaths[i], path) == 0)
        {
            return i;
        }
    }

    return -1;
}

bool FileSystem_AddSearchPath(const char* path)
{
    if (IndexOfSearchPath(path) == -1)
    {
        assert(gSearchPathCount < SEARCH_PATH_MAX_COUNT);
        strcpy(gSearchPaths[gSearchPathCount++], path);
        return true;
    }

    return false;
}

bool FileSystem_RemoveSearchPath(const char* path)
{
    int32_t index = IndexOfSearchPath(path);
    if (index > -1)
    {
        if (index < gSearchPathCount - 1)
        {
            memmove(&gSearchPaths[index], &gSearchPaths[index + 1], gSearchPathCount - index - 1);
        }

        gSearchPathCount--;
        return true;
    }

    return false;
}

bool FileSystem_GetExistsPath(char* buffer, int32_t length, const char* path)
{
    FILE* file = fopen(path, "r");
    if (file)
    {
        fclose(file);
        snprintf(buffer, length, "%s", path);
        return true;
    }

    char tempBuffer[SEARCH_PATH_MAX_LENGTH * 2];
    for (int32_t i = 0; i < gSearchPathCount; i++)
    {
        const char* searchPath = gSearchPaths[i];
        snprintf(tempBuffer, sizeof(tempBuffer), "%s/%s", searchPath, path);
        
        FILE* file = fopen(tempBuffer, "r");
        if (file)
        {
            fclose(file);
            snprintf(buffer, length, "%s", tempBuffer);
            return true;
        }
    }

    return false;
}

//bool FileSystem_GetAbsolutePath(char* buffer, int32_t length, const char* path);
//bool FileSystem_GetRelativePath(char* buffer, int32_t length, const char* path);

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

static bool     StdIO_Open(FileInterface* system, const char* fileNamee, FileMode mode, FileStream* outStream);
static bool     StdIO_Close(FileStream* stream);

static int32_t  StdIO_Read(FileStream* stream, void* outputBuffer, int32_t bufferSizeInBytes);
static int32_t  StdIO_Write(FileStream* stream, const void* inputBuffer, int32_t bufferSizeInBytes);

static bool     StdIO_Seek(FileStream* stream, SeekOffset offset, int32_t count);
static int32_t  StdIO_GetSize(const FileStream* stream);
static int32_t  StdIO_GetCursor(const FileStream* stream);

static bool     StdIO_Flush(FileStream* stream);
static bool     StdIO_IsAtEnd(const FileStream* stream);

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

static FileInterface gStdFileSystem = {
    StdIO_Open,
    StdIO_Close,

    StdIO_Read,
    StdIO_Write,

    StdIO_Seek,
    StdIO_GetSize,
    StdIO_GetCursor,

    StdIO_Flush,
    StdIO_IsAtEnd
};

// -------------------------------------------------------------
// FileStream functions
// -------------------------------------------------------------

//bool FileStream_Open(FileStream* stream, const char* path, FileMode mode)
//{
//    return StdIO_Open(&gStdFileSystem, path, mode, stream);
//}
//
//bool FileStream_Open(FileStream* stream, void* buffer, int32_t bufferSize)
//{
//    return MemoryStream_Open(&gMemoryFileSystem, buffer, bufferSize, stream);
//}

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

bool StdIO_Open(FileInterface* system, const char* fileName, FileMode mode, FileStream* outStream)
{
    char stdMode[6];
    char* ptrMode = stdMode;

    if (mode & FileMode_Read)
    {
        *ptrMode++ = 'r';
    }

    if (mode & FileMode_Write)
    {
        *ptrMode++ = 'w';
    }

    if (mode & FileMode_Binary)
    {
        *ptrMode++ = 'b';
    }

    if (mode & FileMode_Append)
    {
        *ptrMode++ = '+';
    }

    *ptrMode++ = '\0';

    FILE* handle = fopen(fileName, stdMode);
    if (!handle)
    {
        return false;
    }

    FileStream stream;
    stream.inteface = system;
    stream.handle   = handle;

    *outStream = stream;
    return true;
}

bool StdIO_Close(FileStream* stream)
{
    if (stream && stream->inteface == &gStdFileSystem)
    {
        fclose((FILE*)stream->handle);
        stream->handle = NULL;
        return true;
    }

    return false;
}

int32_t StdIO_Read(FileStream* stream, void* outputBuffer, int32_t bufferSizeInBytes)
{
    return (int32_t)fread(outputBuffer, 1, bufferSizeInBytes, (FILE*)stream->handle);
}

int32_t StdIO_Write(FileStream* stream, const void* inputBuffer, int32_t bufferSizeInBytes)
{
    return (int32_t)fwrite(inputBuffer, 1, bufferSizeInBytes, (FILE*)stream->handle);
}

bool StdIO_Seek(FileStream* stream, SeekOffset offset, int32_t count)
{
    long whence;
    switch (offset)
    {
    case SeekOffset_CurrentPosition:
        whence = SEEK_CUR;
        break;

    case SeekOffset_EndOfFile:
        whence = SEEK_END;
        break;

    case SeekOffset_StartOfFile:
        whence = SEEK_SET;
        break;

    default:
        assert(false && "Invalid seek offset");
        whence = -1;
        break;
    }

    return fseek((FILE*)stream->handle, whence, count) > 0;
}

int32_t StdIO_GetSize(const FileStream* stream)
{
    int32_t cursor = ftell((FILE*)stream->handle);

    fseek((FILE*)stream->handle, SEEK_END, 0);
    int32_t size = ftell((FILE*)stream->handle);
    fseek((FILE*)stream->handle, SEEK_SET, cursor);

    return size;
}

int32_t StdIO_GetCursor(const FileStream* stream)
{
    return ftell((FILE*)stream->handle);
}

bool StdIO_Flush(FileStream* stream)
{
    return fflush((FILE*)stream->handle) != 0;
}

bool StdIO_IsAtEnd(const FileStream* stream)
{
    return feof((FILE*)stream->handle);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
