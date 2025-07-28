
#pragma once
#include <cstdlib> 

class vmstat{
    public: 
        vmstat() = default; 
        ~vmstat() = default;

        void printInformation(); 
        //getters and setters 
        size_t getTotalMemory() const { return totalMemory; }
        void setTotalMemory(size_t memory) { totalMemory = memory; }
        
        size_t getMaxMemory() const { return maxMemory; }
        void setMaxMemory(size_t memory) { maxMemory = memory; }

        size_t getFreeMemory() const { return freeMemory; }
        void setFreeMemory(size_t memory) { freeMemory = memory; }

        int getIdleTicks() const { return idleCPUticks; }
        void addIdleTick(){
            idleCPUticks++; 
        }

        int getActiveTicks() const { return activeCPUticks; }
        void addActiveTick(){
            activeCPUticks++;
        }

    private: 
        size_t totalMemory = 0;
        size_t maxMemory = 0; 
        size_t freeMemory = 0; 
        int idleCPUticks = 0; 
        int activeCPUticks = 0;
        int numPagedIn = 0; 
        int numPagedOut = 0; 
};