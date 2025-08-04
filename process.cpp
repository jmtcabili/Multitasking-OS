#include "process.h"
#include "commands/PrintCommand.h"
#include "commands/DeclareCommand.h"
#include "commands/AddCommand.h"
#include "commands/SubtractCommand.h"
#include "commands/SleepCommand.h"
#include "commands/ForCommand.h"
#include <limits>
#include <random>
#include <map>
#include <iostream>
#include <thread>


Process::Process(int pid, std::string name, ScreenConsole* consolePtr)
    : pid(pid), name(name), commandCounter(0), consolePtr(consolePtr), state(READY)
{}

//constructor for memory 
Process::Process(int pid, std::string name, ScreenConsole* consolePtr, size_t memoryRequired, size_t frameSize)
    : pid(pid), name(name), commandCounter(0), consolePtr(consolePtr), 
      state(READY), memoryRequired(memoryRequired), pageSize(frameSize),
      numPages((memoryRequired+frameSize-1)/ frameSize)
{}


void Process::addCommand(ICommand::CommandType type) {
    if (type == ICommand::PRINT) {
        std::string msg = "Hello world from " + name + "!";
        CommandList.push_back(std::make_shared<PrintCommand>(pid, msg));
    }
    else if (type == ICommand::DECLARE) {
        CommandList.push_back(std::make_shared<DeclareCommand>(pid, "x", 0));
    }
    else if (type == ICommand::ADD) {
        CommandList.push_back(std::make_shared<AddCommand>(pid, "var1", "var2", "var3"));
    }
    else if (type == ICommand::SUBTRACT) {
        CommandList.push_back(std::make_shared<SubtractCommand>(pid, "var1", "var2", "var3"));
    }
    /*else {
        std::string msg = "Invalid command type encountered.\n";
    }*/
}

void Process::addCommand(ICommand::CommandType type, uint8_t ticks) {
    if (type == ICommand::SLEEP) {
        CommandList.push_back(std::make_shared<SleepCommand>(pid, ticks));
    } else {
        addCommand(type);
    }
}

void Process::addCommand(const std::vector<std::shared_ptr<ICommand>>& instructions, int repeats) {
    CommandList.push_back(std::make_shared<ForCommand>(pid, instructions, repeats));
}


void Process::executeCurrentCommand(int coreId) const {
    if (commandCounter < CommandList.size()) {
        CommandList[commandCounter]->execute(this->consolePtr, coreId);
    }
}

void Process::moveToNextLine() {
    if (commandCounter < CommandList.size()) {
        ++commandCounter;
    }
}

bool Process::isFinished() const {
    return commandCounter >= CommandList.size();
}

int Process::getRemainingTime() const {
    return CommandList.size() - commandCounter;
}

int Process::getCommandCounter() const {
    return commandCounter;
}

int Process::getLinesOfCode() const {
    return CommandList.size();
}

int Process::getPID() const {
    return pid;
}

int Process::getCPUCoreID() const {
    return cpuCoreID;
}

Process::ProcessState Process::getState() const {
    return state;
}

std::string Process::getName() const {
    return name;
}

void Process::setFinishTime(const std::string& time) {
    timeFinished = time;
}

void Process::setState(Process::ProcessState newState) {
    this->state = newState;
}

std::string Process::getFinishTime() const {
    return timeFinished;
}

size_t Process::getNumPages(){
    return numPages;
}

// allow nested loops with 3 depth
std::vector<std::shared_ptr<ICommand>> generateLoopBody(int pid, int depth, int maxDepth, std::mt19937& rng) {
    std::uniform_int_distribution<int> val(0, 255);
    std::uniform_int_distribution<int> bodySizeDist(2, 4);
    std::uniform_int_distribution<int> cmdChoiceDist(0, 4); // include 4 = possible nested loop
    std::uniform_int_distribution<int> loopCountDist(1, 5);

    std::vector<std::shared_ptr<ICommand>> body;
    int bodySize = bodySizeDist(rng);

    for (int i = 0; i < bodySize; ++i) {
        int choice = cmdChoiceDist(rng);

        if (choice == 0) {
            body.push_back(std::make_shared<PrintCommand>(pid, "Loop print from nested level " + std::to_string(depth)));
        } else if (choice == 1) {
            std::string var = "loopVar" + std::to_string(i);
            body.push_back(std::make_shared<DeclareCommand>(pid, var, val(rng)));
        } else if (choice == 2) {
            body.push_back(AddCommand::generateRandom(pid));
        } else if (choice == 3) {
            body.push_back(SubtractCommand::generateRandom(pid));
        } else if (choice == 4 && depth < maxDepth) {
            int loopCount = loopCountDist(rng);
            auto nestedBody = generateLoopBody(pid, depth + 1, maxDepth, rng);
            body.push_back(std::make_shared<ForCommand>(pid, nestedBody, loopCount));
        }
    }

    return body;
}


void Process::generateTestCaseAlternatingCommands(int count) {
    // Always declare x = 0 first
    CommandList.push_back(std::make_shared<DeclareCommand>(pid, "x", 0));

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> randVal(1, 10);

    for (int i = 0; i < count; ++i) {
        if (i % 2 == 0) {
            std::string msg = "Value from: x";
            CommandList.push_back(std::make_shared<PrintCommand>(pid, msg));
        } else {
            int randomVal = randVal(rng);
            // Generate ADD(x, x, randomValue) as string value
            CommandList.push_back(std::make_shared<AddCommand>(pid, "x", "x", std::to_string(randomVal)));
        }
    }
}



void Process::test_generateRandomCommands(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> instructions(min, max);
    static std::uniform_int_distribution<int> dist(0, 5); // for the various commands
    static std::uniform_int_distribution<int> val(0, 255);
    static std::uniform_int_distribution<int> sleepTicks(1, 10);

    for (int i = 0; i < instructions(rng); ++i) {
        int choice = dist(rng); //random commands
        if (choice == 0) {
            CommandList.push_back(std::make_shared<PrintCommand>(pid, "Hello world from " + name + "!"));
        } else if (choice == 1) {
            std::string var = "var" + std::to_string(i);
            CommandList.push_back(std::make_shared<DeclareCommand>(pid, var, val(rng)));
        } else if (choice == 2) {
            CommandList.push_back(AddCommand::generateRandom(pid));
        } else if(choice == 3){
            CommandList.push_back(SubtractCommand::generateRandom(pid));
        } else if (choice ==4){
            int ticks = sleepTicks(rng);
            CommandList.push_back(std::make_shared<SleepCommand>(pid, ticks));
        } else {
            int loopCount = std::uniform_int_distribution<int>(1, 5)(rng);
            auto loopBody = generateLoopBody(pid, 1, 3, rng); // maxDepth = 3
            CommandList.push_back(std::make_shared<ForCommand>(pid, loopBody, loopCount));
            /*
            old version without nested 
            /Generate a small random body of commands for the ForCommand loop
            int loopCount = std::uniform_int_distribution<int>(1, 5)(rng);

            Create a vector for the body commands
            std::vector<std::shared_ptr<ICommand>> loopBody;

            For demo, let's add 2-4 commands inside the loop
            int bodySize = std::uniform_int_distribution<int>(2, 4)(rng);
            for (int j = 0; j < bodySize; ++j) {
                int cmdChoice = std::uniform_int_distribution<int>(0, 3)(rng);
                if (cmdChoice == 0) {
                    loopBody.push_back(std::make_shared<PrintCommand>(pid, "Loop print from " + name));
                } else if (cmdChoice == 1) {
                    std::string var = "loopVar" + std::to_string(j);
                    loopBody.push_back(std::make_shared<DeclareCommand>(pid, var, val(rng)));
                } else if (cmdChoice == 2) {
                    loopBody.push_back(AddCommand::generateRandom(pid));
                } else if (cmdChoice == 3) {
                    loopBody.push_back(SubtractCommand::generateRandom(pid));
                }
            }
            CommandList.push_back(std::make_shared<ForCommand>(pid, loopBody, loopCount));
            */
        }
    }
}

void Process::screenProcess(int delay){
    //launch thread to run its own
    std::thread executeProcess(&Process::runScreenProcess, this, delay);
    executeProcess.detach();
}

void Process::runScreenProcess(int delay){
    this->state = RUNNING; 
    while (this->state != FINISHED){
        this->executeCurrentCommand(0);
        this->moveToNextLine();
        if (this->isFinished())
            this->state = FINISHED; 
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

size_t Process::getMemoryRequired() const {
    return memoryRequired;
}

void Process::setMemoryPtr(void* ptr) {
    memoryPtr = ptr;
}

void* Process::getMemoryPtr() const {
    return memoryPtr;
}

//symbol table management

// Declare a new variable in the symbol table
bool Process::declareVariable(const std::string& name, uint16_t value) {
    if (variableOffsets.find(name) != variableOffsets.end()) {
        return false; // Already declared
    }

    size_t usedBytes = variableOffsets.size() * sizeof(uint16_t);
    if (usedBytes + sizeof(uint16_t) > SYMBOL_TABLE_SIZE) {
        return false; // No more space
    }

    size_t offset = usedBytes;
    variableOffsets[name] = offset;

    symbolTableMemory[offset] = value & 0xFF;             // low byte
    symbolTableMemory[offset + 1] = (value >> 8) & 0xFF;  // high byte

    return true;
}

// Set the value of an existing variable
bool Process::setVariable(const std::string& name, uint16_t value) {
    auto it = variableOffsets.find(name);
    if (it == variableOffsets.end()) return false;

    size_t offset = it->second;
    symbolTableMemory[offset] = value & 0xFF;
    symbolTableMemory[offset + 1] = (value >> 8) & 0xFF;

    return true;
}

// Get the value of a variable
bool Process::getVariable(const std::string& name, uint16_t& outValue) const {
    auto it = variableOffsets.find(name);
    
    // Check if the variable was found.
    if (it == variableOffsets.end()) {
        // Variable not found, return false to signal failure.
        // We do not modify outValue here.
        return false;
    }

    // Variable was found. Now, get the value.
    size_t offset = it->second;
    // Assuming a little-endian system where the least significant byte is first.
    uint16_t value = symbolTableMemory[offset] | (symbolTableMemory[offset + 1] << 8);

    // Assign the found value to the output parameter.
    outValue = value;
    
    // Return true to signal success.
    return true;
}

// Print the entire symbol table for debugging
void Process::printSymbolTable() const {
    // Gather all variables into a vector for sorting
    std::vector<std::pair<std::string, size_t>> vars(variableOffsets.begin(), variableOffsets.end());

    // Sort by variable name alphabetically
    std::sort(vars.begin(), vars.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    std::cout << "Symbol Table for PID " << pid << ":\n";
    std::cout << std::left << std::setw(12) << "Name"
              << std::setw(10) << "Value"
              << std::setw(10) << "Offset" << "\n";
    std::cout << std::string(32, '-') << "\n";

    for (const auto& [name, offset] : vars) {
        // Read the 2-byte value at the offset (little endian)
        uint16_t value = symbolTableMemory[offset] | (symbolTableMemory[offset + 1] << 8);
        std::cout << std::left << std::setw(12) << name
                  << std::setw(10) << value
                  << std::setw(10) << offset << "\n";
    }
}


// Clear the symbol table when deallocating?
void Process::clearSymbolTable() {
    std::fill(symbolTableMemory.begin(), symbolTableMemory.end(), 0);
    variableOffsets.clear();
}
