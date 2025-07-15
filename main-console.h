#include <string> 
#include "IConsole.h"
#include "config.h"
#include "scheduler.h"
#include "screen-console.h"
#include "FlatMemoryAllocator.h"
#include <map>


#ifndef MAIN_CONSOLE
#define MAIN_CONSOLE

class MainConsole : public IConsole{

    public: 
        MainConsole() = default; //no params needed for constructor
        ~MainConsole() = default; //destructor

        void displayConsoleHeader(); //display default header
        void handleCommand() override; //override for main console command logic
        std::string getUserInput();  //base class for user input

        //command functions 
        void initialize(); 
        void startScheduler(); 
        void stopScheduler();
        void addProcesses(); //add processes to the scheduler
        void generateReport(); 
        void createScreen(std::string& processName); 
        void loadScreen(std::string& processName);
        void listProcesses(); 
        
    private: 
        bool shutdown = false;
        std::unique_ptr<IMemoryAllocator> memoryAllocator;
        bool initialized = false; 
        int processId = 0; 
        std::unique_ptr<Scheduler> runningScheduler = nullptr; 
        std::map<std::string, std::unique_ptr<ScreenConsole>> screenConsoles;// Store multiple screens; let main console manage screen consoles
        emulatorConfig config; // Configuration for the emulator
        std::mutex schedulerMtx; 
        std::mutex IDMtx; 
        int cpuTick = 0; 

};
#endif