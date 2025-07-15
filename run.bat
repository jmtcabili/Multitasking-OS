@echo off
echo Compiling CSOPESY Emulator...

:: Check if g++ is available
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: g++ not found. Please install MinGW or another C++ compiler.
    exit /b 1
)

:: Define source files
set COMMAND_SRCS=commands\AddCommand.cpp commands\DeclareCommand.cpp commands\ForCommand.cpp commands\PrintCommand.cpp commands\SleepCommand.cpp commands\SubtractCommand.cpp
set CORE_SRCS=main.cpp main-console.cpp screen-console.cpp scheduler.cpp process.cpp globals.cpp

:: Compile the program
g++ -std=c++20 -Iinclude %CORE_SRCS% %COMMAND_SRCS% -o main.exe

:: Check for success
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo You can run the emulator by typing: main
) else (
    echo Compilation failed. Please check the errors above.
)