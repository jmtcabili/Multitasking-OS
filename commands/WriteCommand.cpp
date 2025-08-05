#include "WriteCommand.h"
#include <iostream>

// Add 'value' parameter and initialize member 'value'
WriteCommand::WriteCommand(int pid, const std::string& varName, uint32_t memAddress, uint16_t value, Process* proc, PagingAllocator* allocator)
    : ICommand(pid, WRITE), varName(varName), memAddress(memAddress), value(value), process(proc), allocator(allocator) {}


void WriteCommand::execute(ScreenConsole* consolePtr, int coreId) {
    if (memAddress >= allocator->getMaxMemorySize() - 1) {
        if (consolePtr) {
            consolePtr->addLog("WriteCommand: Memory address out of bounds for process " + std::to_string(process->getPID()));
        }
        return;
    }

    // Use the member variable 'value' here:
    allocator->writeUint16(memAddress, value);

    std::string logMessage = "Process " + std::to_string(process->getPID()) + " wrote variable '" + varName + "' to address " + std::to_string(memAddress) + ": " + std::to_string(value);
    if (consolePtr) {
        consolePtr->addLog(logMessage);
    } else {
        std::cout << logMessage << std::endl;
    }
}

