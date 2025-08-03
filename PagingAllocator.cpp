#include "PagingAllocator.h"
#include "backingStore.h"
#include <fstream>
#include <iostream>
#include <sstream>


// initialize allocator with max memory size and frame size
PagingAllocator::PagingAllocator(size_t maxMemorySize, size_t frameSize)
    : maxMemorySize(maxMemorySize), frameSize(frameSize) {
    numFrames = maxMemorySize / frameSize; // calculate number of frames
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i); // initialize free frame list
    }
}

void* PagingAllocator::allocate(Process *process) {
    
    size_t pagesNeeded = process->getNumPages(); //pages needed ni process to run 
    //Process already instatiated with num pages it needs 

    int pid = process->getPID(); //get process id
    for (size_t pageNum = 0; pageNum < pagesNeeded; ++pageNum) {
         handlePageFault(pid, pageNum); // demand paging
    }

    pidToPages[pid].resize(pagesNeeded); //track numpages allocated to pid
    return reinterpret_cast<void*>(process->getPID()); // simulate pointer
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
    loadFromBackingStore(pid, pageNumber, -2); //from backing store //TO DO: Sample lang yung -2
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

    //format: PID | Page # | command counter | cpu core assigned (-1 = not in ram/cpu)
    std::lock_guard <std::mutex> lock(backingStoreMutex); //lock for thread safety
    std::ifstream backingFile("csopesy-backing-store.txt");
    std::vector<std::string> lines; //store backingstore information 

    std::string pageLine; 
    int linePid; 
    size_t linePageNum;
    int lineCore;
    
    try{
        if (backingFile.is_open()) {
            while (std::getline(backingFile, pageLine)){
                lines.push_back(pageLine); //store each line to mem
            }
            backingFile.close(); //close file after reading
        }
        //locate line of interested page
        bool found = false; 
        for (int i = 0; i < lines.size(); i++){
            std::istringstream iss(lines[i]);
            if (iss >> linePid >> linePageNum >> lineCore) { //read PID, page number, and core
                if (linePid == pid && linePageNum == pageNumber) {
                    found = true; //found the page to update
                    lines[i] = std::to_string(linePid) + " " + std::to_string(linePageNum) + " -1"; //update core status
                    break; 
                }
            }
        }

        //overwrite backing store with updated lines
        if (found) {
            std::ofstream out("csopesy-backing-store.txt", std::ios::trunc); //overwrite
            for (const auto& line : lines) {
                out << line << "\n"; //write each line back
            }
            out.close(); //close file after writing
        } else {
            std::cerr << "Page not found in backing store: PID " << pid << ", Page " << pageNumber << std::endl;
        }
    }catch(const std::exception& error){
        std::cerr << "Error reading backing file: " << error.what() << std::endl;
        return; 
    }
}


void PagingAllocator::loadFromBackingStore(int pid, size_t pageNumber, int coreNum) {
    //format: PID | Page # | command counter | cpu core assigned (-1 = not in ram/cpu)
    //can refactor to just have one function for updating backing store? 
    //just include cpu core to be assigned with in bs + option if write / load
    
    std::lock_guard <std::mutex> lock(backingStoreMutex); //lock for thread safety
    std::ifstream backingFile("csopesy-backing-store.txt");
    std::vector<std::string> lines; //store backingstore information 

    std::string pageLine; 
    int linePid; 
    size_t linePageNum;
    int lineCore;
    
    try{
        if (backingFile.is_open()) {
            while (std::getline(backingFile, pageLine)){
                lines.push_back(pageLine); //store each line to mem
            }
            backingFile.close(); //close file after reading
        }
        //locate line of interested page
        bool found = false; 
        for (int i = 0; i < lines.size(); i++){
            std::istringstream iss(lines[i]);
            if (iss >> linePid >> linePageNum >> lineCore) { //read PID, page number, and core
                if (linePid == pid && linePageNum == pageNumber) {
                    found = true; //found the page to update
                    lines[i] = std::to_string(linePid) + " " + std::to_string(linePageNum) + " " + std::to_string(coreNum); //update core status
                    break; 
                }
            }
        }

        //overwrite backing store with updated lines
        if (found) {
            std::ofstream out("csopesy-backing-store.txt", std::ios::trunc); //overwrite
            for (const auto& line : lines) {
                out << line << "\n"; //write each line back
            }
            out.close(); //close file after writing
        } else {
            std::cerr << "Page not found in backing store: PID " << pid << ", Page " << pageNumber << std::endl;
        }
    }catch(const std::exception& error){
        std::cerr << "Error reading backing file: " << error.what() << std::endl;
        return; 
    }
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
void PagingAllocator::snapshot(int quantum) const {
 
}

std::unordered_map<int, std::vector<size_t>> PagingAllocator::getPidOffsets() const {
    return pidToPages;
}
