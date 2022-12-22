#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fs/fs.h>
#include <memory/alloc.h>

// -------------------------------------------------------------
// FileInterface functions prototype
// -------------------------------------------------------------

static bool     stdio_open(FileInterface* system, const char* fileNamee, FileMode mode, FileStream* outStream);
static bool     stdio_close(FileStream* stream);

static int32_t  stdio_read(FileStream* stream, void* outputBuffer, int32_t bufferSizeInBytes);
static int32_t  stdio_write(FileStream* stream, const void* inputBuffer, int32_t bufferSizeInBytes);

static bool     stdio_seek(FileStream* stream, SeekOffset offset, int32_t count);
static int32_t  stdio_get_size(const FileStream* stream);
static int32_t  stdio_get_cursor(const FileStream* stream);

static bool     stdio_flush(FileStream* stream);
static bool     stdio_is_eof(const FileStream* stream);

// -------------------------------------------------------------
// FileInterfaces
// -------------------------------------------------------------

static FileInterface g_stdio_fs = {
    stdio_open,
    stdio_close,

    stdio_read,
    stdio_write,

    stdio_seek,
    stdio_get_size,
    stdio_get_cursor,

    stdio_flush,
    stdio_is_eof
};

// -------------------------------------------------------------
// FileStream functions
// -------------------------------------------------------------

bool fs_open_file(FileStream* stream, const char* path, FileMode mode)
{
	return stdio_open(&g_stdio_fs, path, mode, stream);
}

// -------------------------------------------------------------
// FileInterface functions define
// -------------------------------------------------------------

bool stdio_open(FileInterface* system, const char* file_name, FileMode mode, FileStream* out_stream)
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

    const char* file_path = fs_get_exist_path(file_name);
    FILE* handle = fopen(file_path, stdMode);
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

bool stdio_close(FileStream* stream)
{
    if (stream && stream->inteface == &g_stdio_fs)
    {
        fclose((FILE*)stream->handle);
        stream->handle = NULL;
        return true;
    }

    return false;
}

int32_t stdio_read(FileStream* stream, void* output_buffer, int32_t buffer_size)
{
    return (int32_t)fread(output_buffer, 1, (size_t)buffer_size, (FILE*)stream->handle);
}

int32_t stdio_write(FileStream* stream, const void* input_buffer, int32_t buffer_size)
{
    return (int32_t)fwrite(input_buffer, 1, (size_t)buffer_size, (FILE*)stream->handle);
}

bool stdio_seek(FileStream* stream, SeekOffset offset, int32_t count)
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

    return fseek((FILE*)stream->handle, count, whence) > 0;
}

int32_t stdio_get_size(const FileStream* stream)
{
    int32_t cursor = (int32_t)ftell((FILE*)stream->handle);

    fseek((FILE*)stream->handle, 0, SEEK_END);
    int32_t size = (int32_t)ftell((FILE*)stream->handle);
    fseek((FILE*)stream->handle, cursor, (int)SEEK_SET);

    return size;
}

int32_t stdio_get_cursor(const FileStream* stream)
{
    return (int32_t)ftell((FILE*)stream->handle);
}

bool stdio_flush(FileStream* stream)
{
    return fflush((FILE*)stream->handle) != 0;
}

bool stdio_is_eof(const FileStream* stream)
{
    return (bool)feof((FILE*)stream->handle);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
