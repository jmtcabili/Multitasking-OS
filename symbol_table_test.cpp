#include <iostream>
#include "process.h"
#include "screen-console.h"

/*
Compile using:
g++ -std=c++20 \
symbol_table_test.cpp process.cpp screen-console.cpp globals.cpp PagingAllocator.cpp \
commands/AddCommand.cpp commands/DeclareCommand.cpp commands/ForCommand.cpp commands/PrintCommand.cpp commands/SleepCommand.cpp commands/SubtractCommand.cpp \
-Iinclude -o symbol_table_test
*/

int main() {
    ScreenConsole console(42, "TestProc");
    Process p(42, "TestProc", &console);

    uint16_t val;

    // 1. Declare a variable
    if (p.declareVariable("foo", 1234))
        std::cout << "[PASS] Declared 'foo'\n";
    else
        std::cout << "[FAIL] Failed to declare 'foo'\n";

    // 2. Redeclare same variable – should fail
    if (!p.declareVariable("foo", 8888))
        std::cout << "[PASS] Redeclaring 'foo' correctly failed\n";
    else
        std::cout << "[FAIL] Redeclaring 'foo' should have failed\n";

    // 3. Declare variable at max uint16_t
    if (p.declareVariable("bar", 0xFFFF))
        std::cout << "[PASS] Declared 'bar' with max uint16_t\n";
    else
        std::cout << "[FAIL] Failed to declare 'bar'\n";

    // 4. Set a declared variable
    p.setVariable("foo", 5678);
    val = p.getVariable("foo");
    if (val == 5678)
        std::cout << "[PASS] Set and got 'foo' = 5678\n";
    else
        std::cout << "[FAIL] Expected 'foo' = 5678, got " << val << "\n";

    // 5. Set an undeclared variable – should fail (no exception, just log)
    p.setVariable("baz", 1);
    val = p.getVariable("baz");
    if (val == 0)
        std::cout << "[PASS] Setting undeclared 'baz' had no effect (value = 0)\n";
    else
        std::cout << "[FAIL] Unexpected value for undeclared 'baz': " << val << "\n";

    // 6. Get value of existing variable
    val = p.getVariable("foo");
    if (val == 5678)
        std::cout << "[PASS] Got value of 'foo' = 5678\n";
    else
        std::cout << "[FAIL] Wrong value for 'foo': " << val << "\n";

    // 7. Get value of another existing variable
    val = p.getVariable("bar");
    if (val == 0xFFFF)
        std::cout << "[PASS] Got value of 'bar' = 65535\n";
    else
        std::cout << "[FAIL] Wrong value for 'bar': " << val << "\n";

    // 8. Get value of undeclared variable
    val = p.getVariable("baz");
    if (val == 0)
        std::cout << "[PASS] Getting undeclared 'baz' returned 0\n";
    else
        std::cout << "[FAIL] Unexpected value for 'baz': " << val << "\n";

    // 9. Fill the table to 32 variables
    bool allDeclared = true;
    for (int i = 0; i < 30; ++i) {
        std::string name = "var" + std::to_string(i);
        if (!p.declareVariable(name, i * 10)) {
            std::cout << "[FAIL] Could not declare '" << name << "'\n";
            allDeclared = false;
        }
    }
    if (allDeclared)
        std::cout << "[PASS] Declared 30 additional variables\n";

    // 10. Try to add a 33rd variable
    if (!p.declareVariable("overflow", 0))
        std::cout << "[PASS] Prevented declaring 33rd variable (symbol table full)\n";
    else
        std::cout << "[FAIL] Should not allow 33rd variable\n";

    std::cout << "\n✅ Symbol table test complete.\n\n";

    // Optional: print full symbol table
    p.printSymbolTable();

    return 0;
}
