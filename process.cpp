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
Process::Process(int pid, std::string name, ScreenConsole* consolePtr, size_t memoryRequired)
    : pid(pid), name(name), commandCounter(0), consolePtr(consolePtr), 
      state(READY), memoryRequired(memoryRequired)
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


