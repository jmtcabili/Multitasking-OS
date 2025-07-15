#include "SleepCommand.h"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

SleepCommand::SleepCommand(int pid, uint8_t ticks)
    : ICommand(pid, SLEEP), ticks(ticks) {}

void SleepCommand::execute(ScreenConsole* consolePtr, int coreId) {
    
    consolePtr->addLog("SleepCommand: Sleeping for " + std::to_string(ticks) + " tick(s)...");

    std::this_thread::sleep_for(std::chrono::milliseconds(ticks * 100)); //sleep acts as relinquishing CPU for the specified ticks // can use yield instead maybe?
}
