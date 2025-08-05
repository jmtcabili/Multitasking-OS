#pragma once 
#include <memory>
#include "ICommand.h"
#include <string>
#include <vector>
#include "screen-console.h"


class Process
{
public: 
    enum ProcessState
    {
        READY, 
        RUNNING,
        WAITING,
        FINISHED
    };

    Process(int pid, std::string name, ScreenConsole* consolePtr);
    Process(int pid, std::string name, ScreenConsole* consolePtr, size_t memoryRequired, size_t frameSize);

    
    void addCommand(ICommand::CommandType commandType);
    void addCommand(ICommand::CommandType type, uint8_t ticks); //sleep
    void addCommand(const std::vector<std::shared_ptr<ICommand>>& instructions, int repeats); //for

    void executeCurrentCommand(int coreId) const;
    void moveToNextLine();
    
    bool isFinished() const;
    int getRemainingTime() const;
    int getCommandCounter () const;
    int getLinesOfCode() const;
    int getPID() const;
    int getCPUCoreID() const;
    
    size_t getNumPages(); 

    ProcessState getState() const;
    std::string getName() const;

    void setFinishTime(const std::string& time);
    std::string getFinishTime() const;
    void setState(ProcessState newState);
    void test_generateRandomCommands(int min, int max);
    void screenProcess(int delay); 
    void runScreenProcess(int delay);
    void generateTestCaseAlternatingCommands(int count);
    void createCommandsFromInstructions(const std::string& instructions, int min, int max);

    //memory
    size_t getMemoryRequired() const;
    void setMemoryPtr(void* ptr);
    void* getMemoryPtr() const;

    //symbol table management 
    bool declareVariable(const std::string& name, uint16_t value);
    bool hasVariable(const std::string& name) const;
    uint16_t getVariable(const std::string& name) const;
    void setVariable(const std::string& name, uint16_t value);
    void printSymbolTable() const;
    std::unordered_map<std::string, uint16_t>& getSymbolTable() {
        return symbolTable;
    }



private: 
    int pid;
    std::string name;
    typedef std::vector<std::shared_ptr<ICommand>> Commands;
    Commands CommandList; 
    ScreenConsole* consolePtr; 
    int commandCounter;
    int cpuCoreID = -1;
    std::string timeFinished;
    ProcessState state;

    //memory management
    size_t memoryRequired;
    void* memoryPtr = nullptr;  // Pointer to the allocated memory
    size_t pageSize; 
    size_t numPages; 

    //symbol table
    static const size_t MAX_SYMBOL_TABLE_SIZE = 32;
    std::unordered_map<std::string, uint16_t> symbolTable;

};
