#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "main-console.h"

#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif

using namespace std; 

bool isValidCommand(const std::string& cmd){
  
    if (cmd.empty()) {
        return false; // No command entered
    }

    if (cmd == "initialize" || (cmd.find("screen") == 0) || cmd == "scheduler-start" ||
        cmd == "scheduler-stop" ||  cmd == "report-util" || cmd == "clear" || 
        cmd == "exit") {
        return true; // Valid command
    }

    cout << "Invalid command. Please try again.\n";
    return false; 
}

void MainConsole::handleCommand() { //handleCommand from MainConsole class

    std::string input; 

    while (input != "exit"){
        input = ""; 
        while (!isValidCommand(input)) {
            std::cout << "root:\\\\ ";
            std::getline(std::cin, input);
        } 
        if (input == "initialize"){
            if (this->initialized) {
                std::cout << "Console already initialized.\n";
            } else {
                initialize(); // Call the initialize function to read config and set parameters
            }
        }
        if (input == "exit"){
            std::cout <<  "Terminating main console!" << std::endl;
        } else if(!this->initialized){
            std::cout << "Console not initialized. Please run 'initialize' first.\n";
        } else{
            if (input.find("screen") == 0){//like hw3
                stringstream screenCmd(input);
                string command, option, processName;
                screenCmd >> command >> option >> processName;
                if (option == "-s"){
                    if (processName.empty()){
                        std::cout << "Please provide a process name. (screen -s <process_name>)\n";
                    } else{
                        //TODO: also check if already exists
                        createScreen(processName);
                        std::cout << "Screen " << processName << " created successfully.\n";
                    }
                }else if (option == "-r"){
                    if (processName.empty()){
                        std::cout << "Please provide a process name. (screen -s <process_name>)\n";
                    } if (this->screenConsoles.count(processName) == 0) {
                        std::cout << "Screen " << processName << " does not exist.\n";
                    } else {
                        system(CLEAR_COMMAND);
                        loadScreen(processName); // Load the screen for the process
                    }
                }else if (option == "-ls"){
                    listProcesses();
                }else{
                    std::cout << "Invalid screen command. Use screen -s <process_name>, screen -r <process_name>, or screen -ls\n";
                }
            }else if(input == "scheduler-start"){startScheduler();} //start the scheduler
            else if(input == "scheduler-stop"){stopScheduler();} //stop the scheduler
            else if(input == "report-util"){generateReport();} //generate report
        }
    }
    

}

void MainConsole::initialize(){
    //logic for reading config file and setting scheduler parameters
    ifstream configFile("config.txt"); //fixed naman na siguro yung file name
    std::string configLine; 
    std::string param; 
    std::string value;

    try{
        if (configFile.is_open()) {
            while (getline(configFile, configLine)){
                istringstream ss(configLine); //similar dun sa homework-3, format line as key-value 
                if (ss >> param >> value){
                    if (param == "num-cpu"){
                        this->config.numCores = std::stoi(value); //update the config stuct in the main-console class
                    }
                    else if (param == "scheduler"){
                        this->config.schedulerType = value;
                    }
                    else if (param == "quantum-cycles"){
                        this->config.timeQuantum = std::stoi(value);
                    }
                    else if (param == "batch-process-freq"){
                        this->config.batchFrequency = std::stoi(value);
                    }
                    else if (param == "min-ins"){
                        this->config.minInstructions = std::stoi(value);
                    }
                    else if (param == "max-ins"){
                        this->config.maxInstructions = std::stoi(value);
                    }
                    else if (param == "delay-per-exec"){
                        this->config.delay = std::stoi(value);
                    }
                    else if (param == "max-overall-mem"){
                        this->config.maxMemory = std::stoi(value);
                    }
                    else if (param == "mem-per-frame"){
                        this->config.memoryPerFrame = std::stoi(value);
                    }
                    else if (param == "mem-per-proc"){
                        this->config.memoryPerProc = std::stoi(value);
                    }
                }
            }
        }
        configFile.close();
        this->initialized = true; // Set initialized to true after reading config
        std::cout << "Configuration loaded successfully.\n\n";
        //print all config details 
        std::cout << "Configuration Details:\n";
        std::cout << "Number of CPU Cores: " << this->config.numCores << std::endl;
        std::cout << "Scheduler Type: " << this->config.schedulerType << std::endl;
        std::cout << "Time Quantum: " << this->config.timeQuantum << std::endl;
        std::cout << "Batch Process Frequency: " << this->config.batchFrequency << std::endl;
        std::cout << "Minimum Instructions: " << this->config.minInstructions << std::endl;
        std::cout << "Maximum Instructions: " << this->config.maxInstructions << std::endl;
        std::cout << "Maximum Memory: " << this->config.maxMemory << std::endl;
        std::cout << "Maxmimum Frame:  " << this->config.memoryPerFrame << std::endl;
        std::cout << "Memory per Proc: " << this->config.memoryPerProc << std::endl;

        this->memoryAllocator = std::make_unique<FlatMemoryAllocator>(
            this->config.maxMemory,
            this->config.memoryPerProc,
            this->config.memoryPerFrame
        );
    }catch(const std::exception& error){
        std::cerr << "Error reading config file: " << error.what() << std::endl;
        return; // Exit the function if there's an error
    }
}

//TODO: Impplement
void MainConsole::createScreen(std::string& processName){
    //create process (if no existing)

    int min = this->config.minInstructions;
    int max = this->config.maxInstructions;

    if (this->screenConsoles.count(processName) > 0){
        cout << "Process :" << processName << " already exists" << endl;
        return;
    }

    std::unique_lock<std::mutex> Idlock(this->IDMtx);
    ScreenConsole* screen = new ScreenConsole(this->processId, processName);  //create screen for process
    this->screenConsoles.insert(std::make_pair(processName, screen));   //map processName to screen
    Process* newProcess = new Process(this->processId, processName, screen); //create process with reference to screen]
    
    newProcess->generateTestCaseAlternatingCommands(100000); //generate random commands for the process
    newProcess->setState(Process::READY); // Set the process state to READy
    newProcess->screenProcess(this->config.delay);
    processId++; 
    Idlock.unlock();

}
void MainConsole::loadScreen(std::string& processName){
    //check if screen exists
    if (this->screenConsoles.count(processName) == 0) {
        std::cout << "Screen " << processName << " does not exist.\n";
        return;
    }
    
    //if exists, load the screen console
    ScreenConsole* screen = this->screenConsoles[processName].get(); // Get the screen console pointer
    screen->displayConsoleHeader(); // Display the header for the screen console
    screen->handleCommand(); // Handle commands for the screen console

}
void MainConsole::listProcesses() {
    if (!runningScheduler) {
        std::cout << "Scheduler is not running.\n";
        return;
    }

    int totalCores = config.numCores;
    int usedCores = runningScheduler->getUsedCores();
    int availableCores = totalCores - usedCores;

    // Calculate CPU utilization
    int cpuUtilization = (totalCores > 0) ? (usedCores * 100 / totalCores) : 0;

    std::cout << "\n--- Current Screen Status at T = " << runningScheduler->currentTimestamp() << " ---\n\n";

    std::cout << "CPU Utilization:  " << cpuUtilization << "%\n";
    std::cout << "Cores Used:       " << usedCores << "\n";
    std::cout << "Cores Available:  " << availableCores << "\n";

    std::cout << "\nRunning Processes:\n";
    auto running = runningScheduler->getRunningProcesses();
    if (running.empty()) {
        std::cout << "  None\n";
    } else {
        for (const auto& p : running) {
            std::cout << "  " << p << "\n";
        }
    }

    std::cout << "\nFinished Processes:\n";
    auto finished = runningScheduler->getFinishedProcesses();
    if (finished.empty()) {
        std::cout << "  None\n";
    } else {
        for (const auto& p : finished) {
            std::cout << "  " << p << "\n";
        }
    }

    std::cout << "-------------------------------------------\n";
}

void MainConsole::startScheduler(){
    //instantiate scheduler with config parameters
        //override constructor with either ffcs / rr scheduler
        //only have one running object for the scheduler
    if (this->runningScheduler != nullptr){
         cout << "There is already a running scheduler.\n\n";
         return;
    }
    this->shutdown = false;
    if (this->config.schedulerType == "fcfs") {
        this->runningScheduler = std::make_unique<Scheduler>(
            this->config.numCores, 
            this->config.schedulerType, 
            this->config.delay,
            this->memoryAllocator.get()
        );
    }else if (this->config.schedulerType == "rr"){
        this->runningScheduler = std::make_unique<Scheduler>(
            this->config.numCores, 
            this->config.schedulerType, 
            this->config.timeQuantum,
            this->config.delay,
            this->memoryAllocator.get()
        );
        
    }

    if (this->runningScheduler){
        this->runningScheduler->start(); // Start the scheduler
       
        std::cout << "Scheduler started with " << this->config.numCores << " core(s)\n\n";

        //thread for add processes
        std::thread processThread(&MainConsole::addProcesses, this); // Create a thread to add processes
        processThread.detach();
    }   
}
void MainConsole::addProcesses() {
    int min = this->config.minInstructions;
    int max = this->config.maxInstructions;
    int batch = this->config.batchFrequency;

    while (true) 
    {

        if (cpuTick % batch != 0) 
        {
            cpuTick++;
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
            continue;
        }

        std::unique_lock<std::mutex> idLock(this->IDMtx); 
        std::string processName = "Main-Process" + std::to_string(this->processId);

        ScreenConsole* screen = new ScreenConsole(this->processId, processName);  
        this->screenConsoles.insert(std::make_pair(processName, screen));  

        Process* newProcess = new Process(this->processId, processName, screen, this->config.memoryPerProc);
        newProcess->test_generateRandomCommands(min, max); 
        newProcess->setState(Process::READY); 

        std::unique_lock<std::mutex> lock(schedulerMtx); 
        if (this->runningScheduler){
            this->runningScheduler->addProcess(newProcess); 
        } 
        else 
        {
            delete newProcess; 
            break;
        }

        if(this->shutdown)
        {
            this->runningScheduler->stop();
            break;
        }

        lock.unlock(); 
        idLock.unlock();

        processId++;
        cpuTick++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // slow down generation
    }

}
//TODO: IMPLEMENT
void MainConsole::stopScheduler(){
    this->shutdown = true;
}

void MainConsole::generateReport() {
    if (!this->runningScheduler) {
        std::cout << "Scheduler is not running.\n";
        return;
    }

    std::ofstream outFile("csopesy-log.txt", std::ios::app); // Open in append mode
    if (!outFile.is_open()) {
        std::cerr << "Failed to open log file.\n";
        return;
    }

    std::unique_lock<std::mutex> lock(schedulerMtx); // Ensure thread safety

    int totalCores = config.numCores;
    int usedCores = runningScheduler->getUsedCores();
    int availableCores = totalCores - usedCores;
    int cpuUtilization = (totalCores > 0) ? (usedCores * 100 / totalCores) : 0;

    outFile << "\n--- CPU Report at T = " << runningScheduler->currentTimestamp() << " ---\n\n";

    outFile << "CPU Utilization:  " << cpuUtilization << "%\n";
    outFile << "Cores Used:       " << usedCores << "\n";
    outFile << "Cores Available:  " << availableCores << "\n";

    outFile << "\nRunning Processes:\n";
    auto running = runningScheduler->getRunningProcesses();
    if (running.empty()) {
        outFile << "  None\n";
    } else {
        for (const auto& p : running) {
            outFile << "  " << p << "\n";
        }
    }

    outFile << "\nFinished Processes:\n";
    auto finished = runningScheduler->getFinishedProcesses();
    if (finished.empty()) {
        outFile << "  None\n";
    } else {
        for (const auto& p : finished) {
            outFile << "  " << p << "\n";
        }
    }

    outFile << "-------------------------------------------\n";

    outFile.close(); // Always close the file
    std::cout << "CPU utilization report saved to csopesy-log.txt\n";
    lock.unlock();
}



