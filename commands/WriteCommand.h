#pragma once
#include "../ICommand.h"
#include "../process.h"
#include "../PagingAllocator.h"

class WriteCommand : public ICommand {
public:
    WriteCommand(int pid, const std::string& varName, uint32_t memAddress, uint16_t value, Process* proc, PagingAllocator* allocator);
    void execute(ScreenConsole* consolePtr, int coreId) override;

private:
    std::string varName;
    uint32_t memAddress;
    uint16_t value; 
    Process* process;
    PagingAllocator* allocator;
};
