#include "SubtractCommand.h"
#include <iostream>
#include <limits>
#include <unordered_map>
#include <sstream>
#include <random>

extern std::unordered_map<int, std::unordered_map<std::string, uint16_t>> processVariables;

SubtractCommand::SubtractCommand(int pid, const std::string& target, const std::string& op1, const std::string& op2)
    : ICommand(pid, SUBTRACT), target(target), op1(op1), op2(op2) {}

void SubtractCommand::execute(ScreenConsole* consolePtr, int coreId) {
    auto& vars = processVariables[pid];

    auto getVal = [&](const std::string& arg) -> uint16_t {
        uint16_t val;
        std::istringstream iss(arg);
        if (iss >> val) return val; // it's a literal

        // variable, auto-declare if not found
        if (vars.find(arg) == vars.end()) {
            vars[arg] = 0;
        }
        return vars[arg];
    };

    uint16_t val1 = getVal(op1);
    uint16_t val2 = getVal(op2);
    uint16_t result = (val1 >= val2) ? (val1 - val2) : 0;

    consolePtr->addLog("SubtractCommand: " + op1 + "(" + std::to_string(val1) + ") - " +
                       op2 + "(" + std::to_string(val2) + ") = " + std::to_string(result));

    vars[target] = result;
}

std::shared_ptr<SubtractCommand> SubtractCommand::generateRandom(int pid) {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> pick(0, 1); // 0 = literal, 1 = variable
    static std::uniform_int_distribution<int> val(1, 50); // range for literals TODO: adjust range?
    static std::uniform_int_distribution<int> varId(0, 20); // var0 to var20 TODO: adjust range?, maybe to max instruction value

    auto genArg = [&]() -> std::string {
        if (pick(rng) == 0) {
            return std::to_string(val(rng));  // literal
        } else {
            return "var" + std::to_string(varId(rng));  // variable
        }
    };

    std::string target = "var" + std::to_string(varId(rng));  // where result is stored
    return std::make_shared<SubtractCommand>(pid, target, genArg(), genArg());
}

