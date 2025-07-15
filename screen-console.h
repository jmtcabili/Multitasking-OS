#include <string> 
#include "IConsole.h"
#include <map>

#ifndef SCREEN_CONSOLE
#define SCREEN_CONSOLE

class ScreenConsole : public IConsole{

    public: 
        ScreenConsole(int PID, std::string processName);
        ~ScreenConsole() = default; //destructor

        void displayConsoleHeader(); //display default header
        void handleCommand() override; //override for main console command logic
        std::string getUserInput();  //base class for user input

        //command functions 
        void processSMI();
        void addLog(const std::string& log) {
            logs.push_back(log);
        }
        void exit(); 

    private: 
        std::vector<std::string> logs; 
        int screenID; // Unique identifier for the screen console 
        std::string processName; // Name of the process associated with this screen console


};
#endif