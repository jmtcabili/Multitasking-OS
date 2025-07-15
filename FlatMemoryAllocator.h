#pragma once
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <string>

class FlatMemoryAllocator : public IMemoryAllocator
{
public:
    FlatMemoryAllocator(size_t maxBytes,
                        size_t perProc,
                        size_t frameBytes = 1);

    void* allocate(size_t bytes, int pid) override;
    void  deallocate(int pid)             override;
    std::string visualizeMemory() const   override;
    void snapshot(int quantum)  const     override;

private:
    size_t totalBytes;
    size_t perProcBytes;
    size_t frameBytes;

    std::vector<char> memory;
    std::unordered_map<size_t, bool> allocationMap;
    std::unordered_map<int, std::vector<size_t>> pidToOffsets;

    bool canAllocateAt(size_t index, size_t bytes) const;
};
