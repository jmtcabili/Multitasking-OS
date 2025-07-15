#pragma once
#include "../ICommand.h"
#include <string>
#include <memory>

class AddCommand : public ICommand {
public:
    AddCommand(int pid, const std::string& target, const std::string& op1, const std::string& op2);
    void execute(ScreenConsole* consolePtr, int coreId) override;

    static std::shared_ptr<AddCommand> generateRandom(int pid);  // random generator

private:
    std::string target, op1, op2;
};
