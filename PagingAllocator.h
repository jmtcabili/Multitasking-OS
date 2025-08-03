#pragma once
#include "process.h"
#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>

class PagingAllocator : public IMemoryAllocator {
public:
    PagingAllocator(size_t maxMemorySize, size_t frameSize);

    void* allocate(Process *process) override;
    void deallocate(int pid) override;
    std::string visualizeMemory() const override;

    void handlePageFault(int pid, size_t pageNumber);
    void snapshot(int quantum) const override;
    std::unordered_map<int, std::vector<size_t>> getPidOffsets() const override;

private:
    size_t maxMemorySize;
    size_t frameSize;
    size_t numFrames;

    std::unordered_map<size_t, std::pair<int, size_t>> pageMap; // frame -> (pid, page#)
    std::vector<size_t> freeFrameList;
    std::queue<size_t> fifoQueue; // for FIFO page replacement
    std::unordered_map<int, std::vector<size_t>> pidToPages; // pid -> list of page offsets
    std::mutex allocatorMutex; // mutex for thread safety

    size_t allocateFrame(int pid, size_t pageNumber);
    void evictPage();
    void writeToBackingStore(int pid, size_t pageNumber);
    void loadFromBackingStore(int pid, size_t pageNumber, int coreNum);
};
