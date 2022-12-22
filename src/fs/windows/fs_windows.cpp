#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <assert.h>

#define VC_EXTRA_CLEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fs/fs.h>
#include <memory/alloc.h>

// -------------------------------------------------------------
// FileInterface functions prototype
// -------------------------------------------------------------

static bool     windows_io_open(FileInterface* system, const char* file_name, FileMode mode, FileStream* out_stream);
static bool     windows_io_close(FileStream* stream);

static int32_t  windows_io_read(FileStream* stream, void* output_buffer, int32_t buffer_size);
static int32_t  windows_io_write(FileStream* stream, const void* input_buffer, int32_t buffer_size);

static bool     windows_io_seek(FileStream* stream, SeekOffset offset, int32_t count);
static int32_t  windows_io_get_size(const FileStream* stream);
static int32_t  windows_io_get_cursor(const FileStream* stream);

static bool     windows_io_flush(FileStream* stream);
static bool     windows_io_is_eof(const FileStream* stream);

// -------------------------------------------------------------
// FileInterfaces
// -------------------------------------------------------------

static FileInterface g_windows_io_fs = {
    windows_io_open,
    windows_io_close,

    windows_io_read,
    windows_io_write,

    windows_io_seek,
    windows_io_get_size,
    windows_io_get_cursor,

    windows_io_flush,
    windows_io_is_eof
};

// -------------------------------------------------------------
// FileStream functions
// -------------------------------------------------------------

bool FileStream_OpenFile(FileStream* stream, const char* path, FileMode mode)
{
	return windows_io_open(&g_windows_io_fs, path, mode, stream);
}

// -------------------------------------------------------------
// FileInterface functions define
// -------------------------------------------------------------

bool windows_io_open(FileInterface* system, const char* fileName, FileMode mode, FileStream* out_stream)
{
	DWORD access		= 0;
	DWORD shared		= 0;
	DWORD disposition	= 0;
	DWORD attributes	= 0;

	switch (mode & (FileMode_ReadWrite))
	{
	case FileMode_Read:
		access |= FILE_GENERIC_READ;
		break;

	case FileMode_Write:
		access |= FILE_GENERIC_WRITE;
		break;

	default:
		return 0;
	}

	if (mode & FileMode_Append)
	{
		access &= ~FILE_WRITE_DATA;
		access |= FILE_APPEND_DATA;
	}

	shared = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

	disposition = OPEN_EXISTING;
	//switch (mode & (FileMode_Create | FileMode_Existing | FileMode_Truncate))
	//{
	//case 0:
	//case FileMode_Existing:
	//	disposition = OPEN_EXISTING;
	//	break;
	//
	//case FileMode_Create:
	//	disposition = OPEN_ALWAYS;
	//	break;
	//
	//case FileMode_Create | FileMode_Existing:
	//case FileMode_Create | FileMode_Truncate | FileMode_Existing:
	//	disposition = CREATE_NEW;
	//	break;
	//
	//case FileMode_Truncate:
	//case FileMode_Truncate | FileMode_Existing:
	//	disposition = TRUNCATE_EXISTING;
	//	break;
	//
	//case FileMode_Create | FileMode_Truncate:
	//	disposition = CREATE_ALWAYS;
	//	break;
	//
	//default:
	//	return false;
	//}

	attributes |= FILE_ATTRIBUTE_NORMAL;
	//if (mode & FileMode_Create)
	//{
	//	if (!(mode & FileMode_Write))
	//	{
	//		attributes |= FILE_ATTRIBUTE_READONLY;
	//	}
	//}

	//if (mode & FileMode_Temporary)
	//{
	//	attributes |= FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY;
	//	access |= DELETE;
	//}
	//
	//if (mode & FileMode_ShortLive)
	//{
	//	attributes |= FILE_ATTRIBUTE_TEMPORARY;
	//}

	//switch (mode & (FileMode_Sequence | FileMode_Random))
	//{
	//case 0:
	//	break;
	//
	//case FileMode_Sequence:
	//	attributes |= FILE_FLAG_SEQUENTIAL_SCAN;
	//	break;
	//
	//case FileMode_Random:
	//	attributes |= FILE_FLAG_RANDOM_ACCESS;
	//	break;
	//
	//default:
	//	return false;
	//}

	//if (mode & FileMode_Direct)
	//{
	//	/*
	//	 * FILE_APPEND_DATA and FILE_FLAG_NO_BUFFERING are mutually exclusive.
	//	 * Windows returns 87, ERROR_INVALID_PARAMETER if these are combined.
	//	 *
	//	 * FILE_APPEND_DATA is included in FILE_GENERIC_WRITE:
	//	 *
	//	 * FILE_GENERIC_WRITE = STANDARD_RIGHTS_WRITE |
	//	 *                      FILE_WRITE_DATA |
	//	 *                      FILE_WRITE_ATTRIBUTES |
	//	 *                      FILE_WRITE_EA |
	//	 *                      FILE_APPEND_DATA |
	//	 *                      SYNCHRONIZE
	//	 *
	//	 * Note: Appends are also permitted by FILE_WRITE_DATA.
	//	 *
	//	 * In order for direct writes and direct appends to succeed, we therefore
	//	 * exclude FILE_APPEND_DATA if FILE_WRITE_DATA is specified, and otherwise
	//	 * fail if the user's sole permission is a direct Append, since this
	//	 * particular combination is invalid.
	//	 */
	//	if (access & FILE_APPEND_DATA)
	//	{
	//		if (access & FILE_WRITE_DATA)
	//		{
	//			access &= ~FILE_APPEND_DATA;
	//		}
	//		else
	//		{
	//			return {};
	//		}
	//	}
	//	attributes |= FILE_FLAG_NO_BUFFERING;
	//}

	//switch (mode & (FileMode_DataSync | FileMode_Sync))
	//{
	//case 0:
	//	break;
	//
	//case FileMode::Sync:
	//case FileMode::DataSync:
	//	attributes |= FILE_FLAG_WRITE_THROUGH;
	//	break;
	//
	//default:
	//	return {};
	//}

	/* Setting this flag makes it possible to open a directory. */
	//if (mode & FileMode_Directory)
	//{
	//	attributes |= FILE_FLAG_BACKUP_SEMANTICS;
	//}

	// Async read and write file
	//if (mode & FileMode_NonBlock)
	//{
	//	//attributes |= FILE_FLAG_OVERLAPPED;
	//}

	HANDLE handle = CreateFileA(fileName,
		access,
		shared,
		NULL,
		disposition,
		attributes,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE)
	{
		//DWORD error = GetLastError();
		//if (error == ERROR_FILE_EXISTS && (mode & FileMode_Create) && !(mode & FileMode_Existing))
		//{
		//	/* Special case: when ERROR_FILE_EXISTS happens and UV_FS_O_CREAT was
		//	 * specified, it means the path referred to a directory. */
		//	 //SET_REQ_UV_ERROR(req, UV_EISDIR, error);
		//}
		//else
		//{
		//	//SET_REQ_WIN32_ERROR(req, GetLastError());
		//}
		return false;
	}
	else
	{
		out_stream->handle = (uintptr_t)handle;
		out_stream->inteface = system;
		return true;
	}

	return false;
}

bool windows_io_close(FileStream* stream)
{
    if (stream != nullptr)
    {
		assert(stream->inteface == &g_windows_io_fs);

        CloseHandle((HANDLE)stream->handle);
        stream->handle = (uintptr_t)INVALID_HANDLE_VALUE;
        return true;
    }

    return false;
}

int32_t windows_io_read(FileStream* stream, void* output_buffer, int32_t buffer_size)
{
	DWORD bytesRead;
	if (ReadFile((HANDLE)stream->handle, output_buffer, buffer_size, &bytesRead, nullptr))
	{
		return (int32_t)bytesRead;
	}
	else
	{
		return -1;
	}
}

int32_t windows_io_write(FileStream* stream, const void* input_buffer, int32_t buffer_size)
{
	DWORD bytesWritten;
	if (WriteFile((HANDLE)stream->handle, input_buffer, buffer_size, &bytesWritten, nullptr))
	{
		return (int32_t)bytesWritten;
	}
	else
	{
		return -1;
	}
}

bool windows_io_seek(FileStream* stream, SeekOffset offset, int32_t count)
{
    DWORD moveMethod;
    switch (offset)
    {
    case SeekOffset_CurrentPosition:
		moveMethod = FILE_CURRENT;
        break;

    case SeekOffset_EndOfFile:
		moveMethod = FILE_END;
        break;

    case SeekOffset_StartOfFile:
		moveMethod = FILE_BEGIN;
        break;

    default:
        assert(false && "Invalid seek offset");
		moveMethod = 0;
        break;
    }

	SetFilePointer((HANDLE)stream->handle, count, nullptr, moveMethod);
    return true;
}

int32_t windows_io_get_size(const FileStream* stream)
{
	return GetFileSize((HANDLE)stream->handle, nullptr);
}

int32_t windows_io_get_cursor(const FileStream* stream)
{
    return (int32_t)SetFilePointer((HANDLE)stream->handle, 0, nullptr, FILE_CURRENT);
}

bool windows_io_flush(FileStream* stream)
{
    return (bool)FlushFileBuffers((HANDLE)stream->handle);
}

bool windows_io_is_eof(const FileStream* stream)
{
    return windows_io_get_cursor(stream) == windows_io_get_size(stream);
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
