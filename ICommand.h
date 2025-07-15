#pragma once
#include <sstream>
#include <iomanip>
#include "screen-console.h"
class ICommand
{
    public:
        enum CommandType{
            PRINT, 
            DECLARE, 
            ADD, 
            SUBTRACT, 
            SLEEP, 
            FOR
        };
        ICommand(int pid, CommandType commandType); 
        CommandType getCommandType();
        virtual void execute(ScreenConsole* consolePtr, int coreId) = 0; // Pure virtual function to execute the command -> will override depending on da command 
        std::string getTime();
    protected: 
        int pid; // Process ID
        CommandType commandType; // Type of command
};

inline ICommand::ICommand(int pid, ICommand::CommandType commandType){
    this->pid = pid;
    this->commandType = commandType;
}
inline ICommand::CommandType ICommand::getCommandType(){
    return commandType;
}

inline std::string ICommand::getTime() 
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    // Use std::put_time for formatting the time
    ss << std::put_time(std::localtime(&in_time_t), "(%m/%d/%Y %H:%M:%S %p)");
    return ss.str();
}
