#include <iostream>
#include <cassert>
#include "process.h"

/*
run with  g++ -std=c++20 \   
symbol_table_test.cpp process.cpp screen-console.cpp globals.cpp PagingAllocator.cpp \
commands/AddCommand.cpp commands/DeclareCommand.cpp commands/ForCommand.cpp commands/PrintCommand.cpp commands/SleepCommand.cpp commands/SubtractCommand.cpp \
-Iinclude -o symbol_table_test
*/


int main() {
    // 1) Construct a Process (we only care about symbol table here)
    Process p(42, "TestProc", nullptr /*consolePtr*/);

    // 2) Declare a few variables
    bool ok;
    ok = p.declareVariable("foo", 1234);
    assert(ok);                           // should succeed first time
    ok = p.declareVariable("foo", 9999);
    assert(!ok);                          // redeclaring same name fails

    ok = p.declareVariable("bar", 0xFFFF);
    assert(ok);

    // 3) Set an existing variable
    ok = p.setVariable("foo", 42);
    assert(ok);

    // 4) Try setting a non-existent var
    ok = p.setVariable("baz", 7);
    assert(!ok);

    // 5) Read values back
    uint16_t v;
    ok = p.getVariable("foo", v);
    assert(ok && v == 42);

    ok = p.getVariable("bar", v);
    assert(ok && v == 0xFFFF);

    ok = p.getVariable("baz", v);
    assert(!ok);                          // baz was never declared

    // 6) Fill up the table to its 32-variable limit
    for (int i = 0; i < 30; ++i) {
        std::string name = "v" + std::to_string(i);
        ok = p.declareVariable(name, i);
        assert(ok);
    }
    // 7) One more should fail (out of space)
    ok = p.declareVariable("lastone", 0);
    assert(!ok);

    // 8) Optionally print it out
    p.printSymbolTable();

    std::cout << "All symbol-table tests passed!\n";
    return 0;
}
