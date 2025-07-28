#include "PagingAllocator.h"
#include <iostream>

int main() {
    // Create an allocator with 10 frames
    PagingAllocator allocator(10);

    std::cout << "[1] Initial memory:\n" << allocator.visualizeMemory() << "\n";

    // Simulate allocation by process 1 (e.g., 3 bytes = 3 frames)
    allocator.allocate(3, 1);
    std::cout << "[2] After allocating 3 frames to PID 1:\n" << allocator.visualizeMemory() << "\n";

    // Simulate allocation by process 2 (e.g., 4 bytes = 4 frames)
    allocator.allocate(4, 2);
    std::cout << "[3] After allocating 4 frames to PID 2:\n" << allocator.visualizeMemory() << "\n";

    // Deallocate process 1
    allocator.deallocate(1);
    std::cout << "[4] After deallocating PID 1:\n" << allocator.visualizeMemory() << "\n";

    // Allocate again to see if freed frames are reused
    allocator.allocate(2, 3);
    std::cout << "[5] After allocating 2 frames to PID 3:\n" << allocator.visualizeMemory() << "\n";

    // Deallocate process 2
    allocator.deallocate(2);
    std::cout << "[6] After deallocating PID 2:\n" << allocator.visualizeMemory() << "\n";

    // Deallocate process 3
    allocator.deallocate(3);
    std::cout << "[7] After deallocating PID 3:\n" << allocator.visualizeMemory() << "\n";

    return 0;
}
