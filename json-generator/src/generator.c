// #include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "tinytemplate.h"
#include "stb_ds.h"
#include "parse_structs.h"

typedef struct
{
    StructHash *structs;
    int i;
} StructHashIndex;

typedef struct
{
    Field *fields;
    int i;
} FieldIndex;

typedef struct
{
    tinytemplate_instr_t *prog;
    const char *src;
} Template;

typedef struct
{
    const char *header;
    ParsedStructs parsed_structs;
    FILE *output;
} UserData;

bool field_getter(void *data, const char *key, size_t len, tinytemplate_value_t *value)
{
    FieldIndex *index = (FieldIndex *)data;
    Field *f = index->fields + index->i;

    if (len == 4 && !strncmp(key, "name", len))
    {
        tinytemplate_set_string(value, f->name, strlen(f->name));
        return true;
    }

    if (len == 4 && !strncmp(key, "type", len))
    {
        tinytemplate_set_string(value, f->type, strlen(f->type));
        return true;
    }

    if (len == 9 && !strncmp(key, "type_name", len))
    {
        tinytemplate_set_string(value, f->type_name, strlen(f->type_name));
        return true;
    }

    if (len == 9 && !strncmp(key, "is_object", len))
    {
        tinytemplate_set_int(value, !f->is_primitive);
        return true;
    }

    if (len == 6 && !strncmp(key, "is_int", len))
    {
        tinytemplate_set_int(value, strcmp(f->type, "int") == 0);
        return true;
    }

    if (len == 8 && !strncmp(key, "is_float", len))
    {
        tinytemplate_set_int(value, strcmp(f->type, "float") == 0);
        return true;
    }

    if (len == 7 && !strncmp(key, "is_bool", len))
    {
        tinytemplate_set_int(value, strcmp(f->type, "bool") == 0);
        return true;
    }

    if (len == 9 && !strncmp(key, "is_string", len))
    {
        tinytemplate_set_int(value, f->is_string);
        return true;
    }

    if (len == 8 && !strncmp(key, "is_items", len))
    {
        tinytemplate_set_int(value, f->is_items);
        return true;
    }

    if (len == 6 && !strncmp(key, "is_len", len))
    {
        tinytemplate_set_int(value, f->is_len);
        return true;
    }

    return false;
}

bool next_field(void *data, tinytemplate_value_t *value)
{
    FieldIndex *index = (FieldIndex *)data;

    index->i++;

    if (index->i >= arrlen(index->fields))
    {
        return false;
    }

    tinytemplate_set_dict(value, index, field_getter);

    return true;
}

bool struct_getter(void *data, const char *key, size_t len, tinytemplate_value_t *value)
{
    StructHashIndex *index = (StructHashIndex *)data;
    StructHash *s = index->structs + index->i;

    if (len == 8 && !strncmp(key, "is_array", len))
    {
        tinytemplate_set_int(value, s->value.type_enum == ARRAY);
        return true;
    }

    if (len == 4 && !strncmp(key, "name", len))
    {
        tinytemplate_set_string(value, s->value.type_struct.type_name, strlen(s->value.type_struct.type_name));
        return true;
    }

    if (len == 4 && !strncmp(key, "type", len))
    {
        tinytemplate_set_string(value, s->value.type_struct.type, strlen(s->value.type_struct.type));
        return true;
    }

    if (len == 6 && !strncmp(key, "fields", len))
    {
        FieldIndex *field_index = malloc(sizeof(FieldIndex));

        field_index->fields = s->value.type_struct.fields;
        field_index->i = -1;

        tinytemplate_set_array(value, field_index, next_field);
        return true;
    }

    return false;
}

bool next_struct(void *data, tinytemplate_value_t *value)
{
    StructHashIndex *index = (StructHashIndex *)data;

    index->i++;

    if (index->i >= shlen(index->structs))
    {
        return false;
    }

    tinytemplate_set_dict(value, index, struct_getter);

    return true;
}

bool params_handler(void *data, const char *key, size_t len, tinytemplate_value_t *value)
{
    UserData *user_data = (UserData *)data;

    if (len == 7 && !strncmp(key, "structs", len))
    {
        StructHashIndex *index = malloc(sizeof(StructHashIndex));

        index->structs = user_data->parsed_structs.structs;
        index->i = -1;

        tinytemplate_set_array(value, index, next_struct);
        return true;
    }

    if (len == 6 && !strncmp(key, "header", len))
    {
        tinytemplate_set_string(value, user_data->header, strlen(user_data->header));
        return true;
    }

    return false;
}

void ouput_handler(void *data, const char *str, size_t len)
{
    UserData *user_data = (UserData *)data;

    fwrite(str, 1, len, user_data->output);
}

Template load_template(const char *path)
{
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        perror("Failed to open file");
    }

    // read entire file to a string
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char *)malloc(len);
    len = fread(content, 1, len, file);
    fclose(file);

    tinytemplate_instr_t *prog = malloc(sizeof(tinytemplate_instr_t) * 10000);

    size_t num_instr;
    char message[128];
    tinytemplate_compile(
        content, len,            // Template buffer
        prog, 10000,             // Program buffer
        &num_instr,              // Number of instruction written
        message, sizeof(message) // Error handling
    );

    perror(message);

    return (Template)
    {
        .prog = prog,
        .src = content
    };
}

void generate_code(const char *template_path, const char *output_file, const char *header, ParsedStructs parsed_structs)
{
    Template template = load_template(template_path);

    const char *output_path = malloc(strlen(output_file) + 10 + 1);

    sprintf((char *)output_path, "generated\\%s", output_file);

    FILE *file = fopen(output_path, "w+");

    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }

    UserData user_data = {
        .header = header,
        .parsed_structs = parsed_structs,
        .output = file
    };

    char message[128];
    tinytemplate_eval(
        template.src,   // Template source
        template.prog,  // Template compiled program
        &user_data,     // User data to resolve params and callback
        params_handler, // Callback to resolve params
        ouput_handler,  // Callback to write output
        message,        // Error message
        sizeof(message) // Error message length
    );

    perror(message);

    fclose(file);
}

void generate_parser_code(ParseResult result)
{
    for(int i = 0; i < shlen(result.files); i++)
    {
        const char *header = result.files[i].key;
        ParsedStructs parsed_structs = result.files[i].value;

        generate_code("templates\\cJSON_header.tt", "LDtk_parsers.h", header, parsed_structs);
        generate_code("templates\\cJSON_implementation.tt", "LDtk_parsers.c", header, parsed_structs);
    }
}