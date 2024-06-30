// generate_code.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "platform/platform.h"
#include "parse_structs.h"
#include "generator.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <header_directory>\n", argv[0]);
        return 1;
    }

    const char *header_dir = argv[1];
    ParseResult result = {
        .structs = NULL,
        .file_paths = NULL
    };

    sh_new_arena(result.structs);
    sh_new_arena(result.file_paths);

    parse_headers(header_dir, &result);

    platform_crete_folder("generated");
    
    generate_parser_code(result);
    
    shfree(result.structs);
    shfree(result.file_paths);

    return 0;
}
