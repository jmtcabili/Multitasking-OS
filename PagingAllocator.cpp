#include "PagingAllocator.h"
#include <fstream>
#include <iostream>

// initialize allocator with max memory size and frame size
PagingAllocator::PagingAllocator(size_t maxMemorySize, size_t frameSize)
    : maxMemorySize(maxMemorySize), frameSize(frameSize) {
    numFrames = maxMemorySize / frameSize; // calculate number of frames
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i); // initialize free frame list
    }
}

void* PagingAllocator::allocate(size_t bytes, int pid) {
    size_t pagesNeeded = (bytes + frameSize - 1) / frameSize; //get nearest page 

    for (size_t pageNum = 0; pageNum < pagesNeeded; ++pageNum) {
        handlePageFault(pid, pageNum); // demand paging
    }

    pidToPages[pid].resize(pagesNeeded); //track numpages allocated to pid
    return reinterpret_cast<void*>(pid); // simulate pointer
}

void PagingAllocator::handlePageFault(int pid, size_t pageNumber) {
    // Check if page is already in memory 
    for (const auto& [frame, pair] : pageMap) {
        if (pair.first == pid && pair.second == pageNumber) {
            return; //page loaded already, return 
        }
    }
    // If not, evict a frame
    if (freeFrameList.empty()) {
        evictPage();
    }

    //allocate new frame and simulate loading from backing store
    size_t frame = allocateFrame(pid, pageNumber);
    loadFromBackingStore(pid, pageNumber); //from backing store 
    pageMap[frame] = {pid, pageNumber}; //track mapping
    fifoQueue.push(frame); //add to FIFO queue for replacement
}

//allocate frame from free llist 
size_t PagingAllocator::allocateFrame(int pid, size_t pageNumber) {
    size_t frame = freeFrameList.back(); //get last frame from free list
    freeFrameList.pop_back(); //remove  from free list
    return frame;
}

//remove oldest page and free frame (FIFO replacement)
void PagingAllocator::evictPage() {
    if (fifoQueue.empty()) return; 

    size_t frameToEvict = fifoQueue.front(); //get oldest
    fifoQueue.pop(); //remove from queue 

    auto [victimPid, victimPage] = pageMap[frameToEvict]; //get page info
    writeToBackingStore(victimPid, victimPage); //write to backing store

    pageMap.erase(frameToEvict); //remove from page map
    freeFrameList.push_back(frameToEvict); //mark as free again
}

//TODO: proper format?
void PagingAllocator::writeToBackingStore(int pid, size_t pageNumber) {
    std::ofstream out("csopesy-backing-store.txt", std::ios::app);
    out << "PID " << pid << " PAGE " << pageNumber << " => SWAPPED OUT\n";
    out.close();
}


void PagingAllocator::loadFromBackingStore(int pid, size_t pageNumber) {
    std::ofstream out("csopesy-backing-store.txt", std::ios::app);
    out << "PID " << pid << " PAGE " << pageNumber << " <= LOADED\n";
    out.close();
}


void PagingAllocator::deallocate(int pid) {
    std::vector<size_t> toFree;

    //find frames used by process
    for (const auto& [frame, info] : pageMap) {
        if (info.first == pid) {
            toFree.push_back(frame); //mark frame for freeing
        }
    }

    //remove frames from pageMap and add to free list
    for (size_t frame : toFree) {
        pageMap.erase(frame);
        freeFrameList.push_back(frame);
    }

    pidToPages.erase(pid); //remove page tracking from this process
}

std::string PagingAllocator::visualizeMemory() const {
    std::string output = "Paging Memory Visualization:\n";
    for (size_t i = 0; i < numFrames; ++i) {
        auto it = pageMap.find(i);
        if (it != pageMap.end()) {
            output += "Frame " + std::to_string(i) + " -> PID " +
                      std::to_string(it->second.first) + " PAGE " +
                      std::to_string(it->second.second) + "\n";
        } else {
            output += "Frame " + std::to_string(i) + " -> Free\n";
        }
    }
    return output;
}

//TODO:
/*void PagingAllocator::snapshot(const std::string& filename) const {
 
}*/

std::unordered_map<int, std::vector<size_t>> PagingAllocator::getPidOffsets() const {
    return pidToPages;
}
