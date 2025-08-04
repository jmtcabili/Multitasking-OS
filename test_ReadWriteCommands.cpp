#include <cassert>
#include <iostream>
#include "PagingAllocator.h"
#include "commands/WriteCommand.h"
#include "commands/ReadCommand.h"
#include "screen-console.h"

// Dummy screen console to capture logs
class DummyConsole : public ScreenConsole {
public:
    DummyConsole() : ScreenConsole(0, "DummyProcess") {}
    std::vector<std::string> logs;

    void addLog(const std::string& log) override {
        logs.push_back(log);
        std::cout << log << std::endl;  // Optional: log to console
    }
};

void test_ReadWriteCommands() {
    std::cout << "=== Running test_ReadWriteCommands ===\n";

    // Setup
    const size_t maxMemory = 256;  // bytes
    const size_t frameSize = 64;   // bytes
    PagingAllocator allocator(maxMemory, frameSize);

    DummyConsole console;

    // Setup a fake PID and memory address
    int pid = 1;
    size_t address = 100;
    uint16_t valueToWrite = 12345;
    std::string varName = "myVar";

    // WriteCommand: write value to memory
    WriteCommand writeCmd(pid, varName, address, valueToWrite, &allocator);
    writeCmd.execute(&console, 0);

    // ReadCommand: read value from memory
    ReadCommand readCmd(pid, varName, address, &allocator);
    readCmd.execute(&console, 0);

    // Validation
    uint16_t actualValue = 0;
    bool success = allocator.readFromMemory(address, actualValue);

    assert(success && "Reading from memory should succeed.");
    assert(actualValue == valueToWrite && "Read value should match written value.");

    std::cout << "✅ test_ReadWriteCommands passed!\n\n";
}

int main() {
    test_ReadWriteCommands();
    return 0;
}
