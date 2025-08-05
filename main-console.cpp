#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "VMstat.h"
#include "main-console.h"


#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif

using namespace std; 

MainConsole::~MainConsole() {
    // Set shutdown flag to stop any running threads
   shutdown = true;
    
    if (runningScheduler) {
        runningScheduler->stop();
        runningScheduler->wait();
        runningScheduler.reset();
    }

    screenConsoles.clear();
}

bool isValidCommand(const std::string& cmd){
  
    if (cmd.empty()) {
        return false; // No command entered
    }

    if (cmd == "initialize" || (cmd.find("screen") == 0) || cmd == "scheduler-start" ||
        cmd == "scheduler-stop" ||  cmd == "report-util" || cmd == "clear" || cmd == "process-smi" ||
        cmd == "vmstat" || cmd == "exit") {
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
                string command, option, processName, processMemory, processInstructions;
                screenCmd >> command >> option >> processName >> processMemory;
                getline(screenCmd, processInstructions);
                processInstructions.erase(0, processInstructions.find_first_not_of(" \t"));
                processInstructions.erase(processInstructions.find_last_not_of(" \t") + 1);
                if (option == "-s"){
                    if(checkScreenInput(option, processName, processMemory, processInstructions)){
                        createScreen(processName);
                        std::cout << "Screen " << processName << " created successfully.\n";
                    }
                }
                else if (option == "-r")
                {
                    if(checkScreenInput(option, processName, processMemory, processInstructions)){
                        system(CLEAR_COMMAND);
                        loadScreen(processName); // Load the screen for the process
                    }
                }else if (option == "-ls"){
                    listProcesses();
                }else if(option == "-c"){
                    if(checkScreenInput(option, processName, processMemory, processInstructions)){
                        createScreenWithInstructions(processName, processMemory, processInstructions);
                        std::cout << "Screen " << processName << " created successfully with custom instructions.\n";
                    }
                }
            }
            else if(input == "scheduler-start"){startScheduler();} //start the scheduler
            else if(input == "scheduler-stop"){stopScheduler();} //stop the scheduler
            else if(input == "report-util"){generateReport();} //generate report
            else if(input == "process-smi"){processSMI();}
            else if(input == "vmstat"){printVMStats();}
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
                    else if (param == "min-mem-per-proc"){
                        this->config.minMemProc = std::stoi(value);
                    }
                    else if (param == "max-mem-per-proc"){
                        this->config.maxMemProc= std::stoi(value);
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
        std::cout << "Delay per exec: " << this->config.delay << std::endl; 
        std::cout << "Batch Process Frequency: " << this->config.batchFrequency << std::endl;
        std::cout << "Minimum Instructions: " << this->config.minInstructions << std::endl;
        std::cout << "Maximum Instructions: " << this->config.maxInstructions << std::endl;
        std::cout << "Maximum Memory: " << this->config.maxMemory << std::endl;
        std::cout << "Maxmimum Frame:  " << this->config.memoryPerFrame << std::endl;
        std::cout << "Min mem per Proc: " << this->config.minMemProc << std::endl;
        std::cout << "Max mem per Proc: " << this->config.maxMemProc << std::endl;

        {
            std::lock_guard<std::mutex> lock(vmstatMutex); 
            vmstats.totalMemory = this->config.maxMemory;
        }

        this->memoryAllocator = std::make_unique<PagingAllocator>(
            this->config.maxMemory,
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

void MainConsole::createScreenWithInstructions(std::string& processName, std::string& processMemory, std::string& processInstructions) {
    if (this->screenConsoles.count(processName) > 0) {
        cout << "Process: " << processName << " already exists" << endl;
        return;
    }

    int minIns = this->config.minInstructions;
    int maxIns = this->config.maxInstructions;
    int batch = this->config.batchFrequency;
    
    std::unique_lock<std::mutex> Idlock(this->IDMtx);
    
    // Parse memory size
    size_t min = this->config.minMemProc;
    size_t max = this->config.maxMemProc;
    size_t frameSize = this->config.memoryPerFrame;
    size_t processMem = std::stoi(processMemory);
    
    // Create screen console
    ScreenConsole* screen = new ScreenConsole(this->processId, processName);
    this->screenConsoles.insert(std::make_pair(processName, screen));
    
    // Create process with memory requirements
    Process* newProcess = new Process(this->processId, processName, screen, processMem, frameSize);
    
    // Create commands from instructions
    newProcess->createCommandsFromInstructions(processInstructions, minIns, maxIns);
    
    // Set process state and start
    newProcess->setState(Process::READY);
    newProcess->screenProcess(this->config.delay);
    

    size_t numPages = newProcess->getNumPages();
    std::unique_lock<std::mutex> lock(schedulerMtx); 
    std::ofstream out("csopesy-backing-store.txt", std::ios::app);
    
    for (size_t currPage = 0; currPage < numPages; ++currPage){
        //write to backing store
        int pid = newProcess->getPID();
        int cc = newProcess->getCommandCounter();
        //format: PID | Page # | cpu core assigned (-1 = not in ram/cpu)
        out << pid << " " << currPage << " " << -1 << endl; 
        
    }
    out.close();

    if (this->runningScheduler)
    {
        this->runningScheduler->addProcess(newProcess); 
        processId++;
    } 
    else 
    {
        delete newProcess; 
    }
    Idlock.unlock();
    lock.unlock();
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

    double totalCores = config.numCores;
    double usedCores; 
    double availableCores;

    // Calculate CPU utilization

    std::cout << "\n--- Current Screen Status at T = " << runningScheduler->currentTimestamp() << " ---\n\n";
    auto running = runningScheduler->getRunningProcessDetails(); //running processes on core
    std::unordered_map<int, std::vector<size_t>> pidOffsets = this->memoryAllocator->getPidOffsets(); //running process in memory
    std::unordered_map<size_t, std::pair<int, size_t>> pageMap = this->memoryAllocator->getPageMap(); //get all frames
    std::vector<string> frameProcesses; //store process names for output
    std::vector<std::string> runningProcesses = runningScheduler->getRunningProcesses(); //store process names for output

    double cpuUtilization; 
    if (running.empty()) {
        cout << "  None\n";
    } else {
        //compare running from scheduler and memory 
        usedCores = 0; 
        
        for (const auto& pCore : running) {
            int pid = pCore.first; 
            size_t pagesNeeded = pidOffsets[pid].size(); //get number of pages needed by process
            size_t pagesInFrame = 0; 
            //check if num pages ni process = frames in pagemap
            for (auto& frame : pageMap){
                if (frame.second.first == pid) { //if frame belongs to process
                    pagesInFrame++;                     
                }
            } 
            if (pagesInFrame == pagesNeeded){
                frameProcesses.push_back(pCore.second); 
                usedCores++; 
            }
        }
        availableCores = totalCores - usedCores; 
        cpuUtilization = (totalCores > 0) ? (usedCores * 100 / totalCores) : 0;
    }
    std::cout << "CPU Utilization:  " << cpuUtilization << "%\n";
    std::cout << "Cores Used:       " << usedCores << "\n";
    std::cout << "Cores Available:  " << availableCores << "\n";

    std::cout << "\nRunning Processes:\n";
    if (running.empty()) {
        std::cout << "  None\n";
    } else {
        for (const auto& p : runningProcesses) {
            std::string line = p; 
            std::istringstream iss(line);
            std::string processName;
            std::getline(iss, processName, ' ');
            //if processName is in frameProcesses, print it
            if (std::find(frameProcesses.begin(), frameProcesses.end(), processName) != frameProcesses.end()) {
                std::cout << "  " << p << "\n";
            }
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
    int numProc = 0;
    while (true) 
    {
        if(this->shutdown) //remove in the future
        {
            this->runningScheduler->stop();
            this->runningScheduler->wait();
            break;
        }

        if (cpuTick % batch != 0) 
        {
            cpuTick++;
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
            continue;
        }
        std::unique_lock<std::mutex> idLock(this->IDMtx); 
        std::string processName = "Main-Process" + std::to_string(this->processId);

        ScreenConsole* screen = new ScreenConsole(this->processId, processName);  
        this->screenConsoles.insert(std::make_pair(processName, screen));  


        //generate process with random memory
        size_t minMem = this->config.minMemProc;
        size_t maxMem = this->config.maxMemProc;
        size_t frameSize = this->config.memoryPerFrame;
        size_t processMem = minMem + rand() % (maxMem - minMem + 1);

        
        Process* newProcess = new Process(this->processId, processName, screen, processMem, frameSize);
        newProcess->test_generateRandomCommands(min, max); 
        newProcess->setState(Process::READY); 
        //before process is added, write process details to backing store first
            //write all pages but the first to the backing store
            //write PID, process name, command counter, num pages
        size_t numPages = newProcess->getNumPages();
        std::unique_lock<std::mutex> lock(schedulerMtx); 
        std::ofstream out("csopesy-backing-store.txt", std::ios::app);
       
        for (size_t currPage = 0; currPage < numPages; ++currPage){
            //write to backing store
            int pid = newProcess->getPID();
            int cc = newProcess->getCommandCounter();
            //format: PID | Page # | cpu core assigned (-1 = not in ram/cpu)
            out << pid << " " << currPage << " " << -1 << endl; 
            
        }
        out.close();

        if (this->runningScheduler){
            this->runningScheduler->addProcess(newProcess); 
            numProc++;
        } 
        else 
        {
            delete newProcess; 
            break;
        }


        lock.unlock(); 
        idLock.unlock();

        processId++;
        cpuTick++;
       
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // slow down generation
    }

}

void MainConsole::stopScheduler(){
  
    if (!this->runningScheduler) {
        std::cout << "No scheduler is currently running.\n";
        return;
    }
    
    // Set shutdown flag first to stop process generation
    this->shutdown = true;
    

    
    // Note: The addProcesses thread should terminate automatically 
    // when it checks this->shutdown in its loop
    
    std::cout << "Scheduler stopped successfully.\n";

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
void MainConsole::printVMStats(){

    if (!runningScheduler) {
        std::cout << "Scheduler is not running. Please run 'scheduler-start' first.\n";
        return;
    }
    size_t totalMemUsed = 0; 
    std::unordered_map<size_t, std::pair<int, size_t>> pageMap = this->memoryAllocator->getPageMap(); //get all frames
     for (const auto& frame : pageMap){
        totalMemUsed += config.memoryPerFrame; 
    }

    std::cout << "\n--- VM Statistics at T = " << runningScheduler->currentTimestamp() << " ---\n\n";
    std::lock_guard<std::mutex> lock(vmstatMutex); // Lock to safely access vmstats
    std::cout << "Total Memory: " << vmstats.totalMemory << " MiB\n";
    std::cout << "Used Memory: " << totalMemUsed << " MiB\n";
    std::cout << "Free Memory: " << vmstats.totalMemory-totalMemUsed << " MiB\n";
    std::cout << "Idle CPU Ticks: " << vmstats.idleCPUticks << "\n";
    std::cout << "Active CPU Ticks: " << vmstats.activeCPUticks << "\n";
    std::cout << "Total CPU Ticks: " << vmstats.activeCPUticks + vmstats.idleCPUticks << "\n";
    std::cout << "Page Faults: " << vmstats.numPagedIn << "\n";
    std::cout << "Page Swaps: " << vmstats.numPagedOut << "\n";
}

void MainConsole::processSMI(){
    //NOTE: only considers processes running in cores, not the ones made through screen -s / -c
    if (!runningScheduler) {
        std::cout << "Scheduler is not running. Please run 'scheduler-start' first.\n";
        return;
    }
    
    int totalCores = config.numCores;
    size_t maxMem = config.maxMemory; 
    size_t totalMemUsed = 0; 


    std::unordered_map<int, std::vector<size_t>> pidOffsets = this->memoryAllocator->getPidOffsets(); //running process in memory
    std::unordered_map<size_t, std::pair<int, size_t>> pageMap = this->memoryAllocator->getPageMap(); //get all frames
    std::vector<string> smiProcesses; //store process names for output
    
    for (const auto& frame : pageMap){
        // std::cout << "Frame: " << frame.first << " -> PID: " << frame.second.first 
        //           << ", Page: " << frame.second.second << "\n"; //debugging
        totalMemUsed += config.memoryPerFrame; 
    }

    double totalMemUsedMiB = static_cast<double>(totalMemUsed);
    double maxMemMiB = static_cast<double>(maxMem);
    double memoryUtilPercent = (maxMem > 0) ? (totalMemUsedMiB * 100.0 / maxMemMiB) : 0.0;
    auto running = runningScheduler->getRunningProcessDetails(); //running processes on core
    double cpuUtilization; 
    if (running.empty()) {
        cout << "  None\n";
    } else {
        //compare running from scheduler and memory 
        double usedCores = 0; 
        
        for (const auto& pCore : running) {
            int pid = pCore.first; 
            size_t pagesNeeded = pidOffsets[pid].size(); //get number of pages needed by process
            size_t pagesInFrame = 0; 
            //check if num pages ni process = frames in pagemap 
            for (auto& frame : pageMap){
                if (frame.second.first == pid) { //if frame belongs to process
                    pagesInFrame++;                     
                }
            }
            if (pagesInFrame == pagesNeeded){
                smiProcesses.push_back(pCore.second + " " + std::to_string(pidOffsets[pid].size() * config.memoryPerFrame) + "MiB");
                usedCores++; 
            }
        }
        double availableCores = totalCores - usedCores; 
        cpuUtilization = (totalCores > 0) ? (usedCores * 100 / totalCores) : 0;
    }
    cout << "\n\n";

    cout << "\n--- CPU Report at T = " << runningScheduler->currentTimestamp() << " ---\n\n";

    std::cout << "--------------------------------------------\n";
    std::cout << "| PROCESS-SMI V01.00 Driver Version: 01.00 |\n";
    std::cout << "--------------------------------------------\n\n";
    
    std::cout << "CPU-Util: " << cpuUtilization << "%\n";
    std::cout << "Memory Usage: " << std::fixed << std::setprecision(0) << totalMemUsed << "MiB / "
              << std::fixed << std::setprecision(0) << maxMem << "MiB\n";
    std::cout << "Memory Util: " << std::fixed << std::setprecision(0) << memoryUtilPercent << "%\n";

    std::cout << "============================================\n";
    std::cout << "Running processes and memory usage:\n";
    std::cout << "--------------------------------------------\n";
    if (smiProcesses.empty()) {
        std::cout << "  None\n";
    } else {
        for (const auto& proc : smiProcesses) {
            std::cout << "  " << proc << "\n";
        }
    }
    
}

bool MainConsole::checkScreenInput(std::string& option, std::string& processName, std::string& processMemory, std::string& processInstructions)
{
    if(option.empty())
    {
        std::cout << "Please provide a valid screen option\n";
        return false;
    }

    else if (option == "-r")
    {
        if (processName.empty())
        {
            std::cout << "Please provide a process name. (screen -r <process_name>)\n";
            return false;
        }
        else if (this->screenConsoles.count(processName) == 0) 
        {
            std::cout << "Screen " << processName << " does not exist.\n";
        }
        return true;
    }

    // Common for -s and -c
    bool isScreenCreate = (option == "-s" || option == "-c");
    if (isScreenCreate)
    {
        // Check for duplicates
        if (this->screenConsoles.count(processName) > 0)
        {
            std::cout << "Screen " << processName << " already exists. Use screen -r <process_name> to load it.\n";
            return false;
        }

        // Empty field checks
        else if (processName.empty() || processMemory.empty())
        {
            std::cout << "Please provide a process name and memory size. (screen " << option << " <process_name> <memory_size>)\n";
            return false;
        }

        // Check if memory is numeric and valid
        int mem = 0;
        try {
            mem = std::stoi(processMemory);
        } catch (...) {
            std::cout << "Memory size must be a valid number.\n";
            return false;
        }

        // Memory bounds check
        if (mem < config.minMemProc || mem > config.maxMemProc || (mem & (mem - 1)) != 0)
        {
            std::cout << "Memory size must be a power of 2 between minimum and maximum memory limitations.\n";
            return false;
        }

        // If -c, also check instructions
        else if (option == "-c")
        {
            if (processInstructions.empty()) 
            {
                std::cout << "Please provide quoted semicolon-separated instructions. (screen -c <name> <mem> \"<instructions>\")\n";
                return false;
            }

            processInstructions.erase(std::remove(processInstructions.begin(), processInstructions.end(), '"'), processInstructions.end());

            std::stringstream ss(processInstructions);
            std::string command;
            int count = 0;

            while (std::getline(ss, command, ';'))
            {
                // Trim command
                command.erase(0, command.find_first_not_of(" \t"));
                command.erase(command.find_last_not_of(" \t") + 1);
                if (command.empty()) continue;

                ++count;

                std::stringstream cmdStream(command);
                std::string keyword;
                cmdStream >> keyword;
                std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper);

                // Parse and validate each instruction
                if (keyword == "PRINT") 
                {
                    std::string arg;
                    if (!getline(cmdStream, arg) || arg.empty()) 
                    {
                        std::cout << "invalid command: PRINT requires a message\n";
                        return false;
                    }
                }
                else if (keyword == "DECLARE") 
                {
                    std::string varName;
                    int value;
                    if (!(cmdStream >> varName >> value)) 
                    {
                        std::cout << "invalid command: DECLARE requires <varName> <value>\n";
                        return false;
                    }
                }
                else if (keyword == "ADD" || keyword == "SUBTRACT") 
                {
                    std::string target, op1, op2;
                    if (!(cmdStream >> target >> op1 >> op2)) 
                    {
                        std::cout << "invalid command: " << keyword << " requires <target> <op1> <op2>\n";
                        return false;
                    }
                }
                else if (keyword == "SLEEP") 
                {
                    int ticks;
                    if (!(cmdStream >> ticks) || ticks < 0) 
                    {
                        std::cout << "invalid command: SLEEP requires a positive integer\n";
                        return false;
                    }
                }
                else if (keyword == "WRITE") 
                {
                    std::string address, varName;
                    if (!(cmdStream >> address >> varName)) 
                    {
                        std::cout << "invalid command: WRITE requires <address> <varName>\n";
                        return false;
                    }
                }
                else if (keyword == "READ") 
                {
                    std::string varName, address;
                    if (!(cmdStream >> varName >> address)) 
                    {
                        std::cout << "invalid command: READ requires <varName> <address>\n";
                        return false;
                    }
                }
                else 
                {
                    std::cout << "invalid command: unknown instruction `" << keyword << "`\n";
                    return false;
                }
            }

            if (count < 1 || count > 50) 
            {
                std::cout << "invalid command: instruction count must be between 1 and 50\n";
                return false;
            }
        
            if (count < 1 || count > 50)
            {
                std::cout << "invalid command\n";
                return false;
            }
        }

        return true;
    }

    std::cout << "Invalid screen command. Use -s, -r, or -c.\n";
    return false;
}

