#pragma once
#include "../ICommand.h"
#include <cstdint>  

class SleepCommand : public ICommand {
public:
    SleepCommand(int pid, uint8_t ticks);
    void execute(ScreenConsole* consolePtr,  int coreId) override;

private:
    uint8_t ticks;
};
