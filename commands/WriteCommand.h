#pragma once
#include "../ICommand.h"
#include "../PagingAllocator.h"
#include <string>

class WriteCommand : public ICommand {
public:
    WriteCommand(int pid, const std::string& variableName, size_t memAddress, uint16_t value, PagingAllocator* allocator);
    void execute(ScreenConsole* consolePtr, int coreId) override;

private:
    std::string variableName;
    size_t memAddress;
    uint16_t value;
    PagingAllocator* allocator;
};
