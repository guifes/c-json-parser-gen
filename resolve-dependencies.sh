#!/bin/bash

$MINGWPATH/mingw32-make -C ./tree-sitter
$MINGWPATH/mingw32-make -C ./tree-sitter-c

# Windows
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then

    source_file="./tree-sitter/libtree-sitter.dll"
    destination_folder="./json-generator/lib/windows"

    if [ -e "$source_file" ]; then
        cp "$source_file" "$destination_folder"
        echo "File copied successfully."
    else
        echo "Source file does not exist."
    fi

# macOS
# elif [[ "$OSTYPE" == "darwin"* ]]; then
    
    # source_file="./tree-sitter/libtree-sitter.dll"
    # destination_folder="./json-generator/lib/windows"

    # destination_folder="$macos_destination"

    # if [ -e "$source_file" ]; then
    #     cp "$source_file" "$destination_folder"
    #     echo "File copied successfully."
    # else
    #     echo "Source file does not exist."
    # fi

else
    echo "Unsupported OS."
    exit 1
fi
