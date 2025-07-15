#include "ForCommand.h"

ForCommand::ForCommand(int pid, std::vector<std::shared_ptr<ICommand>> body, int repeatCount)
    : ICommand(pid, FOR), body(body), repeatCount(repeatCount) {}

void ForCommand::execute(ScreenConsole* consolePtr, int coreId) {
    for (int i = 0; i < repeatCount; ++i) {
        consolePtr->addLog("\nForCommand: Loop iteration " + std::to_string(i + 1) + "/" + std::to_string(repeatCount) + "\n");
        for (auto& cmd : body) {
            cmd->execute(consolePtr, coreId);
        }
    }
}