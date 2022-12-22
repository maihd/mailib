#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fs/fs.h>
#include <memory/alloc.h>

constexpr int32_t SEARCH_PATH_MAX_COUNT   = 16;
constexpr int32_t SEARCH_PATH_MAX_LENGTH  = 1024;

static int32_t  g_search_path_count;
static char     g_search_paths[SEARCH_PATH_MAX_COUNT][SEARCH_PATH_MAX_LENGTH];

static int32_t fs_index_of_search_path(const char* path)
{
    for (int32_t i = 0; i < g_search_path_count; i++)
    {
        if (strcmp(g_search_paths[i], path) == 0)
        {
            return i;
        }
    }

    return -1;
}

bool fs_add_search_path(const char* path)
{
    if (fs_index_of_search_path(path) == -1)
    {
        assert(g_search_path_count < SEARCH_PATH_MAX_COUNT);
        strcpy(g_search_paths[g_search_path_count++], path);
        return true;
    }

    return false;
}

bool fs_remove_search_path(const char* path)
{
    const int32_t index = fs_index_of_search_path(path);
    if (index > -1)
    {
        if (index < g_search_path_count - 1)
        {
            memmove(&g_search_paths[index], &g_search_paths[index + 1], g_search_path_count - index - 1);
        }

        g_search_path_count--;
        return true;
    }

    return false;
}

bool fs_exists(const char* relative_path)
{
    FILE* file = fopen(relative_path, "r");
    if (file)
    {
        fclose(file);
        return true;
    }

    return false;
}

bool fs_get_exists_path(char* buffer, int32_t length, const char* relative_path)
{
    if (fs_exists(relative_path))
    {
        snprintf(buffer, length, "%s", relative_path);
        return true;
    }

    char checking_path[SEARCH_PATH_MAX_LENGTH * 2];
    for (int32_t i = 0; i < g_search_path_count; i++)
    {
        const char* search_path = g_search_paths[i];
        snprintf(checking_path, sizeof(checking_path), "%s/%s", search_path, relative_path);
        
        if (fs_exists(checking_path))
        {
            snprintf(buffer, length, "%s", checking_path);
            return true;
        }
    }

    return false;
}

//bool FileSystem_GetAbsolutePath(char* buffer, int32_t length, const char* path);
//bool FileSystem_GetRelativePath(char* buffer, int32_t length, const char* path);

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
