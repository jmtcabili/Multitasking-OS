#include <iostream>
#include <string.h>
#include <cstdlib>
#include <filesystem>

#include "main-console.h"

using namespace std;
#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif


int main() {

    srand(time(0));
    system(CLEAR_COMMAND);
    //clear all files under logs folder

    // Clear or create csopesy-log.txt
    std::ofstream logFile("csopesy-log.txt", std::ofstream::out | std::ofstream::trunc);
    if (logFile.is_open()) {
        logFile << "--- CSOPESY CPU Utilization Log ---\n\n";
        logFile.close();
    } else {
        std::cerr << "Error: Could not create or clear csopesy-log.txt\n";
    }

    std::ofstream backing("csopesy-backing-store.txt", std::ofstream::out | std::ofstream::trunc);
    if (backing.is_open()) {
        backing << "--- CSOPESY BACKING STORE  ---\n\n";
        backing.close();
    } else {
        std::cerr << "Error: Could not create or clear csopesy-backing-store.txt\n";
    }
    
    std::unique_ptr<IConsole> mainConsole = std::make_unique<MainConsole>();
    mainConsole->displayConsoleHeader(); 
    mainConsole->handleCommand(); 

    return 0;

}
