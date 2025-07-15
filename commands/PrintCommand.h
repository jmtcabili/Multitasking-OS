#pragma once
#include "../ICommand.h"
#include <string>

class PrintCommand : public ICommand {
public:
    //  either  a message OR message + variable name
    PrintCommand(int pid, const std::string& message, const std::string& variable = "");
    void execute(ScreenConsole* consolePt, int coreId) override;

private:
    std::string message;
    std::string variable;  // variable name to print after message, empty if none
};
