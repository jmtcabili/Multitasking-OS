#pragma once
#include "../ICommand.h"
#include <string>

class DeclareCommand : public ICommand {
public:
    DeclareCommand(int pid, const std::string& varName, uint16_t value);
    void execute(ScreenConsole* consolePtr, int coreId) override;

private:
    std::string varName;
    uint16_t value;
};