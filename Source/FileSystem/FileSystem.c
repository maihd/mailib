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

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
