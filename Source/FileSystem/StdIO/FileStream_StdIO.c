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

bool FileStream_OpenFile(FileStream* stream, const char* path, FileMode mode)
{
	return StdIO_Open(&gStdFileSystem, path, mode, stream);
}

// -------------------------------------------------------------
// FileInterface functions define
// -------------------------------------------------------------

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
