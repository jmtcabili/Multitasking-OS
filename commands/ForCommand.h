#pragma once
#include "../ICommand.h"
#include <vector>
#include <memory>

class ForCommand : public ICommand {
public:
    ForCommand(int pid, std::vector<std::shared_ptr<ICommand>> body, int repeatCount);
    void execute(ScreenConsole* consolePt, int coreId) override;

private:
    std::vector<std::shared_ptr<ICommand>> body;
    int repeatCount;
};