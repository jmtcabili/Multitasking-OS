#pragma once
#include "process.h"
#include "FlatMemoryAllocator.h"
#include <queue>
#include <mutex>
#include <thread>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <condition_variable>
#include <iostream>

class Scheduler
{
public:

    Scheduler(int coreCount, const std::string& schedulerType, int timeQuantum, int delayPerInstruction, IMemoryAllocator* allocator);
    Scheduler(int coreCount, const std::string& schedulerType, int delayPerInstruction, IMemoryAllocator* allocator);
 
    void addProcess(Process* p);
    void start();
    void stop();
    void wait();

    std::vector<std::string> getCoreStatus();
    std::vector<std::string> getRunningProcesses();
    std::vector<std::string> getFinishedProcesses();
    std::string currentTimestamp(); 
    int getUsedCores();
private:
    int quantumCounter = 0;
    int coreCount;
    std::string schedulerType;         // "FCFS" or "RR"
    int timeQuantum = NULL;                   // For Round Robin
    int delayPerInstruction;           // Execution delay per command (ms)
    IMemoryAllocator* allocator = nullptr;  // Memory Allocator 

    std::queue<Process*> processQueue;
    std::vector<std::thread> coreThreads;
    std::mutex mtx; 
    std::mutex coutMtx; 
    std::condition_variable cv;
    bool shutdown = false;

    std::vector<Process*> cores; 
    std::vector<Process*> finishedProcesses; 

    void coreWorker(int coreId);
};