#include "DeclareCommand.h"
#include <iostream>
#include <limits>
#include <unordered_map>

extern std::unordered_map<int, std::unordered_map<std::string, uint16_t>> processVariables;

DeclareCommand::DeclareCommand(int pid, const std::string& varName, uint16_t value)
    : ICommand(pid, DECLARE), varName(varName), value(value) {}

void DeclareCommand::execute(ScreenConsole* consolePtr, int coreId) {
    auto& vars = processVariables[pid];

    // Check if already declared
    if (vars.find(varName) == vars.end()) {
        vars[varName] = value;
        consolePtr->addLog(this->getTime() + "DeclareCommand: " + varName + " = " + std::to_string(value));
    } else {
        consolePtr->addLog(this->getTime() + "DeclareCommand: " + varName + " already declared with value " + std::to_string(vars[varName]));
    }
}
