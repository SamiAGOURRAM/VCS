#!/bin/bash

# Define the compiler
COMPILER=g++

# Define source files
SOURCES=("VCSController.cpp" "CLI.cpp" "main.cpp")

# Define the output executable name
EXECUTABLE="MiniVCS"

# Compile flags (add any necessary flags, like -std=c++17 for C++17 standard)
COMPILE_FLAGS=""

# Function to compile each source file into an object file
compile() {
    local source_file=$1
    local object_file=${source_file%.cpp}.o

    echo "Compiling $source_file..."
    $COMPILER $COMPILE_FLAGS -c $source_file -o $object_file
}

# Function to link all object files into the final executable
link() {
    echo "Linking object files..."
    $COMPILER *.o -o $EXECUTABLE
}

# Compile each source file
for source in ${SOURCES[@]}; do
    compile $source
done

# Link object files
link

echo "Compilation completed. Executable: $EXECUTABLE"

# Clean up object files
echo "Cleaning up..."
rm *.o

echo "Done."
