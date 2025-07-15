#include "AddCommand.h"
#include <iostream>
#include <limits>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <random>

extern std::unordered_map<int, std::unordered_map<std::string, uint16_t>> processVariables;

AddCommand::AddCommand(int pid, const std::string& target, const std::string& op1, const std::string& op2)
    : ICommand(pid, ADD), target(target), op1(op1), op2(op2) {}

void AddCommand::execute(ScreenConsole* consolePtr, int coreId) {
    auto& vars = processVariables[pid];

    // Helper to get a value from either a literal or variable
    auto getVal = [&](const std::string& arg) -> uint16_t {
        uint16_t val;
        std::istringstream iss(arg);
        if (iss >> val) return val;  // Literal number (e.g., "5")

        // If variable doesn't exist, auto-declare with 0
        if (vars.find(arg) == vars.end()) {
            vars[arg] = 0;
        }
        return vars[arg];
    };

    // Ensure target variable exists
    if (vars.find(target) == vars.end()) {
        vars[target] = 0;  // Auto-declare if needed
    }

    uint16_t val1 = getVal(op1);
    uint16_t val2 = getVal(op2);
    uint16_t result = val1 + val2;

    if (result > std::numeric_limits<uint16_t>::max()) {
        result = std::numeric_limits<uint16_t>::max();
    }

    consolePtr->addLog("AddCommand: " + target + " = " + op1 + "(" + std::to_string(val1) + ") + " +
                       op2 + "(" + std::to_string(val2) + ") => " + std::to_string(result));

    vars[target] = result;
}

std::shared_ptr<AddCommand> AddCommand::generateRandom(int pid) {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> pick(0, 1); // 0 = literal, 1 = variable
    static std::uniform_int_distribution<int> val(1, 50); // literal values TODO: adjust range???
    static std::uniform_int_distribution<int> varId(0, 20); // for var0 to var20 TODO: adjust range?, maybe to max instruction value

    auto genArg = [&]() -> std::string {
        if (pick(rng) == 0) {
            return std::to_string(val(rng));  // literal
        } else {
            return "var" + std::to_string(varId(rng));  // declared or undeclared variable
        }
    };

    std::string target = "var" + std::to_string(varId(rng));  // result variable
    return std::make_shared<AddCommand>(pid, target, genArg(), genArg());
}
