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

void deleteDirectoryContents(const std::filesystem::path& dir)
{
    for (const auto& entry : std::filesystem::directory_iterator(dir)) 
        std::filesystem::remove_all(entry.path());
    //https://stackoverflow.com/questions/59077670/c-delete-all-files-and-subfolders-but-keep-the-directory-itself
    //don't mind error in vscode, just compile with c++17 or later
}

int main() {
  
    system(CLEAR_COMMAND);
    //clear all files under logs folder
    deleteDirectoryContents("logs");

    // Clear or create csopesy-log.txt
    std::ofstream logFile("csopesy-log.txt", std::ofstream::out | std::ofstream::trunc);
    if (logFile.is_open()) {
        logFile << "--- CSOPESY CPU Utilization Log ---\n\n";
        logFile.close();
    } else {
        std::cerr << "Error: Could not create or clear csopesy-log.txt\n";
    }
    
    std::unique_ptr<IConsole> mainConsole = std::make_unique<MainConsole>();
    mainConsole->displayConsoleHeader(); 
    mainConsole->handleCommand(); 

    return 0;

}
