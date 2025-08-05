#include <iostream>
#include <iomanip>
#include "PagingAllocator.h"
#include "process.h"
#include "commands/ReadCommand.h"
#include "screen-console.h"

void printSymbolTable(const Process& p) {
    std::cout << "\n--- Symbol Table ---" << std::endl;
    for (const auto& [name, value] : p.getSymbolTable()) {
        std::cout << name << " = 0x" << std::hex << std::setw(4) << std::setfill('0') << value << std::endl;
    }
}

void printPhysicalMemory(const PagingAllocator& allocator) {
    std::cout << "\n--- Physical Memory ---" << std::endl;
    const auto& memory = allocator.getPhysicalMemory();
    for (size_t i = 0; i < memory.size(); ++i) {
        if (i % 16 == 0)
            std::cout << "\n0x" << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(memory[i]) << " ";
    }
    std::cout << "\n" << std::dec; // reset formatting
}

int main() {
    // Setup allocator with 64 bytes memory, frame size 16
    PagingAllocator allocator(64, 16);

    // Setup a dummy console for logs
    ScreenConsole console(42, "TestProc");

    // Setup a dummy process with PID 42, symbol table etc.
    Process p(42, "TestProc", &console);

    // Directly write a value to address 0x10 in physicalMemory (e.g. value = 0xABCD)
    uint32_t testAddress = 0x10;
    uint16_t testValue = 0xABCD;

    allocator.writeUint16(testAddress, testValue);
    
    uint16_t readBack = allocator.readUint16(testAddress);
    std::cout << "Written value: 0x" << std::hex << testValue
              << ", Read back value: 0x" << std::hex << readBack << std::endl;

    // Create ReadCommand to read from testAddress into var "myVar"
    ReadCommand readCmd(p.getPID(), "myVar", testAddress, &p, &allocator);

    // Execute the read command
    readCmd.execute(&console, 0);

    // Check the value stored in the process symbol table
    uint16_t storedValue = p.getVariable("myVar");

    std::cout << "Expected value: 0x" << std::hex << testValue << std::endl;
    std::cout << "Stored value in symbol table: 0x" << std::hex << storedValue << std::endl;

    if (storedValue == testValue) {
        std::cout << "\n✅ ReadCommand test PASSED" << std::endl;
    } else {
        std::cout << "\n❌ ReadCommand test FAILED" << std::endl;
    }

    printSymbolTable(p);
    printPhysicalMemory(allocator);

    return 0;
}
