#include "ReadCommand.h"
#include <iostream>

ReadCommand::ReadCommand(int pid, const std::string& variableName, size_t memAddress, PagingAllocator* allocator)
    : ICommand(pid, READ), variableName(variableName), memAddress(memAddress), allocator(allocator) {}

void ReadCommand::execute(ScreenConsole* consolePtr, int coreId) {
    uint16_t value;
    if (allocator->readFromMemory(memAddress, value)) {
        consolePtr->addLog("ReadCommand: Variable '" + variableName + "' = " + std::to_string(value));
    } else {
        consolePtr->addLog("ReadCommand: Failed to read memory at address " + std::to_string(memAddress));
    }
}
