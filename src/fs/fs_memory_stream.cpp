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

static bool     memory_stream_open(FileInterface* inteface, void* buffer, int32_t buffer_size, FileStream* out_stream);
static bool     memory_stream_close(FileStream* inteface);

static int32_t  memory_stream_read(FileStream* stream, void* output_buffer, int32_t buffer_size);
static int32_t  memory_stream_write(FileStream* stream, const void* input_buffer, int32_t buffer_size);

static bool     memory_stream_seek(FileStream* stream, SeekOffset offset, int32_t count);
static int32_t  memory_stream_get_size(const FileStream* stream);
static int32_t  memory_stream_get_cursor(const FileStream* stream);

static bool     memory_stream_flush(FileStream* stream);
static bool     memory_stream_is_eof(const FileStream* stream);

// -------------------------------------------------------------
// FileInterfaces
// -------------------------------------------------------------

static FileInterface g_memory_stream_fs = {
    memory_stream_open,
    memory_stream_close,

    memory_stream_read,
    memory_stream_write,

    memory_stream_seek,
    memory_stream_get_size,
    memory_stream_get_cursor,

    memory_stream_flush,
    memory_stream_is_eof
};

// -------------------------------------------------------------
// FileStream functions
// -------------------------------------------------------------

bool fs_from_memory(FileStream* stream, void* buffer, int32_t buffer_size)
{
	return memory_stream_open(&g_memory_stream_fs, buffer, buffer_size, stream);
}

// -------------------------------------------------------------
// FileInterface functions define
// -------------------------------------------------------------

bool memory_stream_open(FileInterface* inteface, void* input_buffer, int32_t buffer_size, FileStream* out_stream)
{
    FileStream stream;
    stream.inteface      = inteface;
    stream.memory.buffer = input_buffer;
    stream.memory.length = buffer_size;
    stream.memory.cursor = 0;
    stream.memory.flags  = MemoryStreamFlags_None;

    *out_stream = stream;
    return true;
}

bool memory_stream_close(FileStream* stream)
{
    if (stream && stream->inteface == &g_memory_stream_fs)
    {
        if (stream->memory.flags & MemoryStreamFlags_BufferOwner)
        {
            //Memory_Free(stream->memory.buffer);
        }

        return true;
    }

    return false;
}

int32_t memory_stream_read(FileStream* stream, void* output_buffer, int32_t buffer_size)
{
    const int32_t read_bytes = stream->memory.cursor + buffer_size > stream->memory.length 
        ? stream->memory.length - stream->memory.cursor
        : buffer_size;

    memcpy(output_buffer, (uint8_t*)stream->memory.buffer + stream->memory.cursor, read_bytes);
    stream->memory.cursor += read_bytes;
    return read_bytes;
}

int32_t memory_stream_write(FileStream* stream, const void* input_buffer, int32_t buffer_size)
{
    const int32_t written_bytes = stream->memory.cursor + buffer_size > stream->memory.length
        ? stream->memory.length - stream->memory.cursor
        : buffer_size;

    memcpy((uint8_t*)stream->memory.buffer + stream->memory.cursor, input_buffer, written_bytes);
    stream->memory.cursor += written_bytes;
    return written_bytes;
}

bool memory_stream_seek(FileStream* stream, SeekOffset offset, int32_t count)
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

int32_t memory_stream_get_size(const FileStream* stream)
{
    return stream->memory.length;
}

int32_t memory_stream_get_cursor(const FileStream* stream)
{
    return stream->memory.cursor;
}

bool memory_stream_flush(FileStream* stream)
{
    return true;
}

bool memory_stream_is_eof(const FileStream* stream)
{
    return stream->memory.cursor == stream->memory.length;
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
