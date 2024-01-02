#!/bin/bash

# Define the name of the output executable
OUTPUT_NAME="out"

SRC_DIR="src"
# Get all C source files in the src directory
SOURCE_FILES=$(find "$SRC_DIR" -type f -name "*.c")

# Compiler options
CC="gcc"                 # Compiler command
CFLAGS="-Wall -g"        # Compilation flags (e.g., -Wall for warnings, -g for debugging info)

# Linker options
LDFLAGS="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"

# Check if output directory exists, create if not
OUTPUT_DIR="bin"
if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir "$OUTPUT_DIR"
fi

# Compile and link the C project
echo "Compiling and linking..."
$CC $CFLAGS $SOURCE_FILES -o "$OUTPUT_DIR/$OUTPUT_NAME" $LDFLAGS

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable: $OUTPUT_DIR/$OUTPUT_NAME"
else
    echo "Compilation failed."
fi
