#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "screen-console.h"

using namespace std; 

bool isValidCommandS(const std::string& cmd){
  
    if (cmd.empty()) {
        return false; // No command entered
    }

    if (cmd == "process-smi" || cmd == "exit") {
        return true; // Valid command
    }

    cout << "Invalid command. Please try again.\n";
    return false; 
}

ScreenConsole::ScreenConsole(int PID, string processName)
{
    this->screenID = PID; 
    this->processName = processName;
}

void ScreenConsole::displayConsoleHeader()
{
    cout << "Entered screen: " << processName << endl;
}

void ScreenConsole::handleCommand() { //handleCommand from MainConsole class

    std::string input; 

    while (input != "exit"){
        input = ""; 
        while (!isValidCommandS(input)) {
            std::cout << "screen-root:\\\\ ";
            std::getline(std::cin, input);
        } 
        if (input == "process-smi")
            processSMI(); 
        else if (input == "exit") {
            break;
        }
    }
}

void ScreenConsole::processSMI() {
    cout << "Process name: " << this->processName << endl; 
    cout << "ID: " << this->screenID << endl; 

    int length = this->logs.size(); 

    for (int i = 0; i < length; i++){
        cout << this->logs[i] << endl; 
    }
}



