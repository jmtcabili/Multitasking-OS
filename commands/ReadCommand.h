#pragma once
#include "../ICommand.h"
#include "../process.h"
#include "../PagingAllocator.h"

class ReadCommand : public ICommand {
public:
    ReadCommand(int pid, const std::string& varName, uint32_t memAddress, Process* proc, PagingAllocator* allocator);

    void execute(ScreenConsole* consolePtr, int coreId) override;

private:
    std::string varName;
    uint32_t memAddress;
    Process* process;
    PagingAllocator* allocator;
};
