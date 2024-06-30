#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include "stdio.h"
#include "platform.h"

#define MAX_PATH_LEN 1024

//////////////
// Internal //
//////////////

void _traverse_directory_recursively(const char *root_path, const char *path, FilePathHandler handler, void *user_data)
{
    char dir_spec[MAX_PATH];
    snprintf(dir_spec, MAX_PATH, "%s\\*", path);

    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    hFind = FindFirstFile(dir_spec, &find_file_data);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("FindFirstFile failed for path %s (%lu)\n", path, GetLastError());
        return;
    }

    do
    {
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(find_file_data.cFileName, ".") != 0 && strcmp(find_file_data.cFileName, "..") != 0)
            {
                char sub_dir[MAX_PATH];
                snprintf(sub_dir, MAX_PATH, "%s\\%s", path, find_file_data.cFileName);
                
                _traverse_directory_recursively(root_path, sub_dir, handler, user_data);
            }
        }
        else
        {
            char file_path[MAX_PATH];
            snprintf(file_path, MAX_PATH, "%s\\%s", path, find_file_data.cFileName);

            const char *relative_file_path = file_path + strlen(root_path) + 1;

            handler(file_path, relative_file_path, user_data);
        }
        
    } while (FindNextFile(hFind, &find_file_data) != 0);

    FindClose(hFind);
}

////////////
// Public //
////////////

const char *platform_get_absolute_path(const char *relative_path, const char *absolute_path)
{
    if (_fullpath((char *)absolute_path, relative_path, MAX_PATH_LEN) == NULL)
    {
        perror("Error resolving absolute path");
    }
}

void platform_traverse_directory_recursively(const char *path, FilePathHandler handler, void *user_data)
{
    char full_path[MAX_PATH] = {0};
    
    // Convert the relative path to an absolute path
    platform_get_absolute_path(path, full_path);
    
    _traverse_directory_recursively(full_path, full_path, handler, user_data);
}

void platform_crete_folder(const char *path)
{
    _mkdir("generated");
}

#endif