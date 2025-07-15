#pragma once
#include <string>

enum class AllocationStrategy { //for mo2 future proofing
    FirstFit
    //BestFit,
    //WorstFit
};

class IMemoryAllocator {
public:
    virtual void* allocate(size_t bytes, int pid) = 0;
    virtual void  deallocate(int pid) = 0;
    virtual std::string visualizeMemory() const = 0;
    virtual void snapshot(int quantum) const = 0;

    virtual ~IMemoryAllocator() = default;
};
