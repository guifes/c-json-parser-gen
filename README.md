# c-json-parser-gen
An utility for generating JSON parsing code in C from struct definitions

## Submodule Dependencies

- Tree Sitter (https://github.com/tree-sitter/tree-sitter)
- Tree Sitter C Grammar (https://github.com/tree-sitter/tree-sitter-c)
- Tiny Template (https://github.com/guifes/tinytemplate) Fork of (https://github.com/cozis/tinytemplate)
- LLVM (https://github.com/llvm/llvm-project)
- stb_ds.h (https://github.com/nothings/stb)

## Description

This project uses [Tree-Sitter ](https://tree-sitter.github.io/tree-sitter/) to parse C code and feed into [Tiny Template ](https://github.com/guifes/tinytemplate) to allow for the generation of arbitrary parsing JSON parsing code. [LLVM](https://github.com/llvm/llvm-project) is used to format the output.

## Examples

Check the *templates* folder for an example of a template to generate cJON parsers.

## Tiny Template Data

The utility feeds the following struct information into templates:

| param   | type   | description |
| ------- | ------ | - |
| header  | string | The relative path to the header containing the current struct definition |
| structs | array  | The array of Struct objects |

### Struct

| attribute | type   | description |
| --------- | ------ | - |
| type      | string | The name of the struct in the C definition
| type_name | string | The name of the struct in the C definition without specifiers |
| is_array  | bool   | True if the struct represents and array |

### Field

| attribute | type   | description |
| --------- | ------ | - |
| name      | string | The name of the attribute in the C definition |
| type      | string | The field type |
| type_name | string | The field type without specifiers |
| is_int    | bool   | True is the field is integer |
| is_float  | bool   | True if the field is float |
| is_bool   | bool   | True if the field is bool |
| is_string | bool   | True if the field is string |
| is_object | bool   | True if the field is a struct (including arrays) |
| is_items  | bool   | True if the field is the *items* attribute in an array |
| is_len    | bool   | True if the field is the *len* attribute in an array |

## Representing Arrays with Structs

This program understands the following struct structure as an array:

```c
struct SomeArray {
    AnyType *items;
    size_t len;
};
```

It doesn't enforce the attributes names. Only enforces the attribute count of *2* and one of the attributes being of type *size_t*.