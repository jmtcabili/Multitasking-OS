@echo off
setlocal enabledelayedexpansion

echo Compiling CSOPESY Emulator...

:: Check if g++ is available
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: g++ not found. Please install MinGW or another C++ compiler.
    exit /b 1
)

echo Using compiler: g++

:: Define include directory and output path
set "INCLUDE_DIR=include"
set "OUTPUT=main.exe"

:: Define command source files
set "COMMAND_SRCS=^
commands\AddCommand.cpp ^
commands\DeclareCommand.cpp ^
commands\ForCommand.cpp ^
commands\PrintCommand.cpp ^
commands\SleepCommand.cpp ^
commands\SubtractCommand.cpp"

:: Define core source files
set "CORE_SRCS=^
main.cpp ^
main-console.cpp ^
screen-console.cpp ^
scheduler.cpp ^
process.cpp ^
globals.cpp ^
PagingAllocator.cpp"

:: Compile all sources
g++ -std=c++20 -Wall -Wextra -I%INCLUDE_DIR% %CORE_SRCS% %COMMAND_SRCS% -o %OUTPUT%

:: Check if compilation succeeded
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo You can run the emulator by typing: %OUTPUT%
) else (
    echo Compilation failed. Please check the errors above.
)

endlocal
