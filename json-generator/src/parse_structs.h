#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    const char *type;
    const char *type_name;
    const char *name;
    bool is_primitive;
    bool is_string;
    bool is_pointer;
    bool is_items;
    bool is_len;
} Field;

typedef struct
{
    const char *type;
    const char *type_name;
    Field *fields;
} TypeStruct;

typedef enum {
    NONE,
    OBJECT,
    ARRAY
} TypeEnum;

typedef struct {
    TypeStruct type_struct;
    TypeEnum type_enum;
} TypeData;

typedef struct
{
    char *key;
    TypeData value;
} StructHash;

typedef struct
{
    char *key;
    bool value;
} FilePathHash;

typedef struct
{
    StructHash *structs;
    FilePathHash *file_paths;
} ParseResult;

void parse_headers(const char *path, ParseResult *parse_result);
