#!/bin/bash

# Exit the script on any command with non-0 return code
set -e

# Define the compiler (GCC)
COMPILER=g++

# Define the output executable name
EXECUTABLE="test"

# Define the include directory
INCLUDE_DIR="./build/include"

# Define the source directory
SRC_DIR="./build/src"

# Define the build directory
BUILD_DIR="./build"

# Define the test source file (outside the src directory)
TEST_SRC="./test.cpp"

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Compile command with C++17 support
COMPILE_CMD="$COMPILER -I$INCLUDE_DIR -c -std=c++17"

# Compile the test source file
echo "Compiling test source file..."
$COMPILE_CMD $TEST_SRC -o $BUILD_DIR/test.o

# Find all cpp files in the src directory
CPP_FILES=$(find $SRC_DIR -name "*.cpp")

# Compile each source file
for file in $CPP_FILES; do
    FILENAME=$(basename $file)
    OBJECT_FILE="${FILENAME%.cpp}.o"
    echo "Compiling $file..."
    $COMPILE_CMD $file -o $BUILD_DIR/$OBJECT_FILE
done

# Link the object files into the final executable with C++17 support
echo "Linking..."
$COMPILER -o $EXECUTABLE $BUILD_DIR/*.o -std=c++17

# Cleanup
echo "Cleaning up object files..."
rm $BUILD_DIR/*.o

echo "Compilation and linking completed."
echo "Executable created at ./$EXECUTABLE"

# End of the script