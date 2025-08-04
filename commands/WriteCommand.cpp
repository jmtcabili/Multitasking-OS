#include "WriteCommand.h"
#include <iostream>

WriteCommand::WriteCommand(int pid, const std::string& variableName, size_t memAddress, uint16_t value, PagingAllocator* allocator)
    : ICommand(pid, WRITE), variableName(variableName), memAddress(memAddress), value(value), allocator(allocator) {}

void WriteCommand::execute(ScreenConsole* consolePtr, int coreId) {
    if (allocator->writeToMemory(memAddress, value)) {
        consolePtr->addLog("WriteCommand: Wrote value " + std::to_string(value) + " to '" + variableName + "'");
    } else {
        consolePtr->addLog("WriteCommand: Failed to write to memory at address " + std::to_string(memAddress));
    }
}
