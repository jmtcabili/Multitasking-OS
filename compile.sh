#!/bin/bash

echo "Compiling CSOPESY Emulator..."

# Check which compiler is available (prefer clang++ on macOS)
if command -v clang++ &> /dev/null
then
    CXX="clang++"
elif command -v g++ &> /dev/null
then
    CXX="g++"
else
    echo "Error: Neither clang++ nor g++ found. Please install a C++ compiler."
    exit 1
fi

# Compile all .cpp files and link them
# command files 
COMMAND_SRCS="commands/AddCommand.cpp commands/DeclareCommand.cpp commands/ForCommand.cpp commands/PrintCommand.cpp commands/SleepCommand.cpp commands/SubtractCommand.cpp"
CORE_SRCS="main.cpp main-console.cpp screen-console.cpp scheduler.cpp process.cpp globals.cpp FlatMemoryAllocator.cpp"

"$CXX" -std=c++20 -Iinclude $CORE_SRCS $COMMAND_SRCS -o main

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "You can run the emulator by typing: ./main"
else
    echo "Compilation failed. Please check the errors above."
fi