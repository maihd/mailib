#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct FileInterface FileInterface;

typedef enum FileMode
{
    FileMode_None               = 0,
    FileMode_Read               = 1 << 0,
    FileMode_Write              = 1 << 1,
    FileMode_Append             = 1 << 2,
    FileMode_Binary             = 1 << 3,

    FileMode_ReadBinary         = FileMode_Read | FileMode_Binary,
    FileMode_WriteBinary        = FileMode_Write | FileMode_Binary,

    FileMode_ReadWrite          = FileMode_Read | FileMode_Write,
    FileMode_ReadWriteBinary    = FileMode_Read | FileMode_Write | FileMode_Binary,
} FileMode;

typedef enum SeekOffset
{
    SeekOffset_StartOfFile,
    SeekOffset_CurrentPosition,
    SeekOffset_EndOfFile,
} SeekOffset;

typedef enum MemoryStreamFlags
{
	MemoryStreamFlags_None			= 0,
    MemoryStreamFlags_BufferOwner	= 1 << 0
} MemoryStreamFlags;

typedef struct MemoryStream
{
    void*               buffer;
    int32_t             length;
    int32_t             cursor;
    MemoryStreamFlags   flags;
} MemoryStream;

typedef struct FileStream
{
    FileInterface*      inteface;
    union
    {
        uintptr_t       handle;
        MemoryStream    memory;
    };
} FileStream;

struct FileInterface
{
    void*                 const open_fn;
    bool                (*const close_fn)(FileStream* stream);

    int32_t             (*const read_fn)(FileStream* stream, void* output_buffer, int32_t buffer_size);
    int32_t             (*const write_fn)(FileStream* stream, const void* input_buffer, int32_t buffer_size);

    bool                (*const seek_fn)(FileStream* stream, SeekOffset offset, int32_t position);
    int32_t             (*const get_size_fn)(const FileStream* stream);
    int32_t             (*const get_cursor_fn)(const FileStream* stream);
    
    bool                (*const flush_fn)(FileStream* stream);
    bool                (*const is_eof_fn)(const FileStream* stream);
};

// -------------------------------------------------------------
// FileSystem functions
// -------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

bool            fs_add_search_path(const char* path);
bool            fs_remove_search_path(const char* path);

bool            fs_exists(const char* relative_path);
bool            fs_get_exists_path(char* buffer, int32_t length, const char* relative_path);

//bool            FileSystem_GetAbsolutePath(char* buffer, int32_t length, const char* path);
//bool            FileSystem_GetRelativePath(char* buffer, int32_t length, const char* path);

//bool            FileSystem_LoadZipFile(const char* path);
//bool            FileSystem_UnloadZipFile(const char* path);

#ifdef __cplusplus
}
#endif

// -------------------------------------------------------------
// FilePath functions
// -------------------------------------------------------------

// -------------------------------------------------------------
// FileStream functions
// -------------------------------------------------------------

bool fs_open_file(FileStream* stream, const char* path, FileMode mode);
bool fs_from_memory(FileStream* stream, void* buffer, int32_t buffer_size);

inline bool fs_close(FileStream* stream)
{
    return stream->inteface->close_fn(stream);
}

inline int32_t fs_read(FileStream* stream, void* outputBuffer, int32_t buffer_size)
{
    return stream->inteface->read_fn(stream, outputBuffer, buffer_size);
}

inline int32_t fs_write(FileStream* stream, const void* input_buffer, int32_t buffer_size)
{
    return stream->inteface->write_fn(stream, input_buffer, buffer_size);
}

inline bool fs_seek(FileStream* stream, SeekOffset offset, int32_t position)
{
    return stream->inteface->seek_fn(stream, offset, position);
}

inline int32_t fs_get_size(const FileStream* stream)
{
    return stream->inteface->get_size_fn(stream);
}

inline int32_t fs_get_cursor(const FileStream* stream)
{
    return stream->inteface->get_cursor_fn(stream);
}

inline bool fs_flush(FileStream* stream)
{
    return stream->inteface->flush_fn(stream);
}

inline bool fs_is_eof(const FileStream* stream)
{
    return stream->inteface->is_eof_fn(stream);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
