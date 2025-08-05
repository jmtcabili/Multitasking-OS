#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class AllocationStrategy { //for mo2 future proofing
    FirstFit
    //BestFit,
    //WorstFit
};

class IMemoryAllocator {
public:
    virtual void* allocate(Process *process) = 0;
    virtual void  deallocate(int pid) = 0;
    virtual std::string visualizeMemory() const = 0;
    virtual void snapshot(int quantum) const = 0;
    virtual ~IMemoryAllocator() = default;
    virtual std::unordered_map<int, std::vector<size_t>> getPidOffsets() const = 0; 
    virtual std::vector<size_t> getFreeFrameList() = 0; // Get free frame list for debugging
    virtual std::unordered_map<size_t, std::pair<int, size_t>> getPageMap() = 0; // Get page map for debugging
    virtual bool checkProcessFrames(Process *process) = 0;
};
