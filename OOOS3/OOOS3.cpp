#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;

void PrintMemoryStatus() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);

    if (GlobalMemoryStatusEx(&memStatus)) {
        cout << "Total virtual memory: "
            << memStatus.ullTotalVirtual / (1024 * 1024) << " MB" << endl;
        cout << "Available virtual memory: "
            << memStatus.ullAvailVirtual / (1024 * 1024) << " MB" << endl;
        cout << "Used: "
            << (memStatus.ullTotalVirtual - memStatus.ullAvailVirtual) / (1024 * 1024)
            << " MB" << endl;
    }
}

int main() {
    vector<LPVOID> allocatedBlocks;
    const SIZE_T BLOCK_SIZE = 100 * 1024 * 1024;

    cout << "=== VIRTUAL MEMORY FRAGMENTATION RESEARCH ===" << endl;

    // POINT 1
    cout << "\n[POINT 1] Initial state:" << endl;
    PrintMemoryStatus();
    cout << "Press Enter to start..." << endl;
    cin.get();

    // POINT 2
    cout << "\n[POINT 2] Allocating 100 MB blocks:" << endl;
    int blockCount = 0;

    while (true) {
        LPVOID block = VirtualAlloc(NULL, BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (block == NULL) {
            cout << "Failed to allocate block " << (blockCount + 1) << endl;
            break;
        }

        allocatedBlocks.push_back(block);
        blockCount++;
        memset(block, 0xA, BLOCK_SIZE);
        cout << "Allocated block " << blockCount << endl;
    }

    cout << "Total blocks allocated: " << blockCount << endl;

    // POINT 3
    cout << "\n[POINT 3] After allocating all blocks:" << endl;
    PrintMemoryStatus();
    cout << "Press Enter to continue..." << endl;
    cin.get();

    // POINT 4
    cout << "\n[POINT 4] Freeing every second block:" << endl;

    for (size_t i = 0; i < allocatedBlocks.size(); i += 2) {
        if (VirtualFree(allocatedBlocks[i], 0, MEM_RELEASE)) {
            allocatedBlocks[i] = nullptr;
            cout << "Freed block " << (i + 1) << endl;
        }
    }

    // Clean vector
    vector<LPVOID> temp;
    for (void* block : allocatedBlocks) {
        if (block != nullptr) {
            temp.push_back(block);
        }
    }
    allocatedBlocks = temp;

    // POINT 5
    cout << "\n[POINT 5] After freeing every second block:" << endl;
    PrintMemoryStatus();
    cout << "Press Enter to continue..." << endl;
    cin.get();

    // POINT 6 - Fragmentation test
    cout << "\n[POINT 6] Fragmentation test:" << endl;
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    GlobalMemoryStatusEx(&memStatus);
    SIZE_T availableMemory = memStatus.ullAvailVirtual;

    LPVOID largeBlock = VirtualAlloc(NULL, availableMemory, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (largeBlock != NULL) {
        cout << "SUCCESS: Allocated large block" << endl;
        VirtualFree(largeBlock, 0, MEM_RELEASE);
    }
    else {
        DWORD error = GetLastError();
        cout << "ERROR: Failed to allocate large block!" << endl;
        cout << "Error code: " << error << endl;
        cout << "Proof of fragmentation!" << endl;
    }

    // POINT 7
    cout << "\n[POINT 7] Freeing all remaining blocks:" << endl;
    for (void* block : allocatedBlocks) {
        VirtualFree(block, 0, MEM_RELEASE);
    }
    allocatedBlocks.clear();
    PrintMemoryStatus();

    cout << "\nPress Enter to continue to second part..." << endl;
    cin.get();

    // POINT 8 - Already done (final state)

    // POINT 9 - Memory reservation
    cout << "\n[POINT 9] Memory reservation:" << endl;

    SIZE_T sizes[] = { 100 * 1024 * 1024, 50 * 1024 * 1024, 10 * 1024 * 1024 };
    const char* sizeNames[] = { "100 MB", "50 MB", "10 MB" };

    for (int i = 0; i < 3; i++) {
        void* block = VirtualAlloc(NULL, sizes[i], MEM_RESERVE, PAGE_READWRITE);
        if (block != NULL) {
            cout << "SUCCESS: Reserved " << sizeNames[i] << " block" << endl;

            // Try to commit part of it
            if (VirtualAlloc(block, sizes[i] / 2, MEM_COMMIT, PAGE_READWRITE) != NULL) {
                cout << "Successfully committed half of the reserved block" << endl;
                VirtualFree(block, 0, MEM_RELEASE);
            }
            break;
        }
        else {
            cout << "Failed to reserve " << sizeNames[i] << endl;
        }
    }

    cout << "\nExperiment completed!" << endl;
    cout << "Press Enter to exit..." << endl;
    cin.get();

    return 0;
}