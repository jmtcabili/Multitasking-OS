#include "FlatMemoryAllocator.h"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxBytes,
                                         size_t perProc,
                                         size_t frame)
    : totalBytes(maxBytes),
      perProcBytes(perProc),
      frameBytes(frame),
      memory(maxBytes, '.')
{
    for (size_t i = 0; i < maxBytes; ++i)
        allocationMap[i] = false;
}

void* FlatMemoryAllocator::allocate(size_t bytes, int pid)
{
    if (bytes > perProcBytes) return nullptr;

    for (size_t i = 0; i <= totalBytes - bytes; ++i)
    {
        if (!allocationMap[i] && canAllocateAt(i, bytes))
        {
            for (size_t j = 0; j < bytes; ++j) {
                allocationMap[i + j] = true;
                memory[i + j] = static_cast<char>('A' + (pid % 26));
                pidToOffsets[pid].push_back(i + j);
            }
            return reinterpret_cast<void*>(i); // simulate address
        }
    }

    return nullptr;
}

void FlatMemoryAllocator::deallocate(int pid)
{
    auto it = pidToOffsets.find(pid);
    if (it == pidToOffsets.end()) return;

    for (size_t offset : it->second) {
        allocationMap[offset] = false;
        memory[offset] = '.';
    }

    pidToOffsets.erase(it);
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t bytes) const
{
    for (size_t j = 0; j < bytes; ++j)
        if (allocationMap.at(index + j)) return false;
    return true;
}

std::string FlatMemoryAllocator::visualizeMemory() const
{
    return std::string(memory.begin(), memory.end()); //to print logs 
}

void FlatMemoryAllocator::snapshot(int quantum) const
{
    // timestamp
    std::time_t t = std::time(nullptr);
    char ts[64];
    std::strftime(ts, sizeof(ts), "(%m/%d/%Y %I:%M:%S%p)", std::localtime(&t));

    std::ofstream out("memory_stamp_" + std::to_string(quantum) + ".txt");

    // external fragmentation calculation
    int procCount = pidToOffsets.size();
    size_t extFrag = 0, freeRun = 0;

    for (size_t i = 0; i < totalBytes; ++i)
    {
        if (!allocationMap.at(i)) {
            ++freeRun;
        } else {
            if (freeRun && freeRun < perProcBytes) extFrag += freeRun;
            freeRun = 0;
        }
    }
    if (freeRun && freeRun < perProcBytes) extFrag += freeRun;

    out << "Timestamp: " << ts << '\n';
    out << "Number of processes in memory: " << procCount << '\n';
    out << "Total external fragmentation in KB: " << extFrag / 1024 << "\n\n";

    // memory map top-down
    out << "----end---- = " << totalBytes << '\n';

    // Gather memory blocks
    std::vector<std::pair<size_t, int>> blocks; // pair<start, pid>
    for (const auto& pair : pidToOffsets) {
        int pid = pair.first;
        const std::vector<size_t>& offsets = pair.second;
        if (!offsets.empty()) {
            size_t start = *std::min_element(offsets.begin(), offsets.end());
            blocks.emplace_back(start, pid);
        }
    }

    // Sort blocks by starting address descending (top of memory first)
    std::sort(blocks.begin(), blocks.end(), [](auto& a, auto& b) {
        return a.first > b.first;
    });

    for (const auto& block : blocks)
    {
        size_t start = block.first;
        size_t end = start + perProcBytes;

        out << end << '\n';
        out << "P" << block.second << '\n';
        out << start << '\n';
    }

    out << "----start---- = 0\n";
}

