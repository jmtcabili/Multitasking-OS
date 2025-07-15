#include <string>
#ifndef CONFIG
#define CONFIG

struct emulatorConfig{
    int numCores; 
    std::string schedulerType; // FCFS / RR
    int timeQuantum; // For Round Robin scheduling
    int batchFrequency; 
    int minInstructions; 
    int maxInstructions;
    int delay; 

    //memory fields
    size_t maxMemory;       
    size_t memoryPerFrame;  
    size_t memoryPerProc;
}; 
#endif