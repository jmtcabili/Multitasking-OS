#include "PrintCommand.h"
#include <iostream>
#include <fstream>
#include <unordered_map>


extern std::unordered_map<int, std::unordered_map<std::string, uint16_t>> processVariables;

PrintCommand::PrintCommand(int pid, const std::string& message, const std::string& variable)
    : ICommand(pid, PRINT), message(message), variable(variable) {}

void PrintCommand::execute(ScreenConsole* consolePtr, int coreId) {

    //file logging instead of cout

    try{
        consolePtr->addLog(this->getTime() + "Core: " + std::to_string(coreId) + " PrintCommand: " + message);
    }catch(const std::exception& error) {
        std::cerr << "Error writing to log file for PID " << pid << ": " << error.what() << std::endl;

    }
    
    if (!variable.empty()) {
        auto& vars = processVariables[pid];
        if (vars.count(variable)) {
            std::cout << vars[variable];
        } else {
            std::cout << "(undefined variable: " << variable << ")";
        }
    }
}
