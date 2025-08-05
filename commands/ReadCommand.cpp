#include "ReadCommand.h"
#include <iostream>

ReadCommand::ReadCommand(int pid, const std::string& varName, uint32_t memAddress, Process* proc, PagingAllocator* allocator)
    : ICommand(pid, READ), varName(varName), memAddress(memAddress), process(proc), allocator(allocator) {}

void ReadCommand::execute(ScreenConsole* consolePtr, int coreId) {
    if (memAddress >= allocator->getMaxMemorySize() - 1) {
        if (consolePtr) {
            consolePtr->addLog("ReadCommand: Memory address out of bounds for process " + std::to_string(process->getPID()));
        }
        return;
    }

    uint16_t value = allocator->readUint16(memAddress);
    process->setVariable(varName, value); // Store into symbol table
    std::string logMessage = "Process " + std::to_string(process->getPID()) + " read variable '" + varName + "' from address " + std::to_string(memAddress) + ": " + std::to_string(value);
    if (consolePtr) {
        consolePtr->addLog(logMessage);
    } else {
        std::cout << logMessage << std::endl; // Fallback to console output if no ScreenConsole
    }
}