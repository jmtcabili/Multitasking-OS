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
    Process(int pid, std::string name, ScreenConsole* consolePtr, size_t memoryRequired);

    
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

    ProcessState getState() const;
    std::string getName() const;

    void setFinishTime(const std::string& time);
    std::string getFinishTime() const;
    void setState(ProcessState newState);
    void test_generateRandomCommands(int min, int max);
    void screenProcess(int delay); 
    void runScreenProcess(int delay);
    void generateTestCaseAlternatingCommands(int count);

    //memory
    size_t getMemoryRequired() const;
    void setMemoryPtr(void* ptr);
    void* getMemoryPtr() const;

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

};
