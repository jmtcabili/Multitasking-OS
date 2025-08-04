#pragma once
#include "../ICommand.h"
#include "../PagingAllocator.h"
#include <string>

class ReadCommand : public ICommand {
public:
    ReadCommand(int pid, const std::string& variableName, size_t memAddress, PagingAllocator* allocator);
    void execute(ScreenConsole* consolePtr, int coreId) override;

private:
    std::string variableName;
    size_t memAddress;
    PagingAllocator* allocator;
};
