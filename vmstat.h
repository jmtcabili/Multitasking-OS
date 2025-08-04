#pragma once
struct VMstat{
        size_t totalMemory = 0;
        size_t maxMemory = 0; 
        size_t freeMemory = 0; 
        int idleCPUticks = 0; 
        int activeCPUticks = 0;
        int numPagedIn = 0; 
        int numPagedOut = 0; 
};