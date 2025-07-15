#pragma once
#include "../ICommand.h"
#include <string>

class SubtractCommand : public ICommand {
public:
    SubtractCommand(int pid, const std::string& target, const std::string& op1, const std::string& op2);
    void execute(ScreenConsole* consolePtr, int coreId) override;

    static std::shared_ptr<SubtractCommand> generateRandom(int pid);


private:
    std::string target, op1, op2;
};
