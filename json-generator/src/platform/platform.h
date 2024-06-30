#pragma once

typedef void (*FilePathHandler)(const char *, const char *, void *user_data);

void         platform_traverse_directory_recursively (const char *path, FilePathHandler handler, void *user_data);
const char * platform_get_absolute_path              (const char *relative_path, const char *absolute_path);
void         platform_crete_folder                   (const char *path);