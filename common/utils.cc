#include "utils.h"

#include <winternl.h>

#include <iostream>

#include "structs.h"

namespace utils {

uint64_t utilities::GetKernelBaseAddress() {
    ULONG len = 0;
    NTSTATUS(NTAPI * NtQuerySystemInformation)
    (const int SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength,
     PULONG ReturnLength) =
        (NTSTATUS(NTAPI*)(const int, PVOID, ULONG, PULONG))GetProcAddress(
            GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");

    NTSTATUS status = NtQuerySystemInformation(SystemModuleInformation, nullptr, 0, &len);
    if (len == 0) {
        std::cout << "[-]Failed to get the correct length for system information." << std::endl;
        return 1;
    }

    // Allocate buffer for the modules information
    std::vector<char> buffer(len);
    PSYSTEM_MODULE_INFORMATION moduleInfo =
        reinterpret_cast<PSYSTEM_MODULE_INFORMATION>(buffer.data());

    // Query again using the correct size
    status = NtQuerySystemInformation(SystemModuleInformation, moduleInfo, len, &len);
    if (status != 0 /* STATUS_SUCCESS */) {
        std::cout << "[-] NtQuerySystemInformation failed." << std::endl;
        return 1;
    }

    // Print the base address of the first module, typically the kernel
    std::cout << "[-] Kernel Base Address: 0x" << std::hex << moduleInfo->Modules[0].Base
              << std::endl;
    return reinterpret_cast<uint64_t>(moduleInfo->Modules[0].Base);
}

void dumpBuffer(void* ptr, size_t size) {
    auto* bytePtr = static_cast<const unsigned char*>(ptr);
    std::cout << "Memory dump from: 0x" << std::hex << ptr << std::endl;
    std::cout << std::hex << std::setfill('0');

    for (size_t i = 0; i < size; ++i) {
        // Print the memory address and the hex values in groups of 16 bytes
        if (i % 16 == 0) {
            if (i != 0)
                std::cout << std::endl;
            std::cout << "0x" << std::setw(8) << reinterpret_cast<const void*>(bytePtr + i) << ": ";
        }

        std::cout << std::setw(2) << static_cast<unsigned>(bytePtr[i]) << " ";

        // Print ASCII values at the end of each line
        if (i % 16 == 15 || i == size - 1) {
            std::cout << " " << std::endl;
            for (size_t j = i - i % 16; j <= i; ++j)
                std::cout << (std::isprint(bytePtr[j]) ? static_cast<char>(bytePtr[j]) : '.');
        }
    }
    std::cout << std::dec << std::endl;
}

int utilities::CreateProcessWrapper(LPCTSTR lpApplicationName) {
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;

    // CreateProcess parameters
    LPTSTR lpCommandLine = NULL;
    LPSECURITY_ATTRIBUTES lpProcessAttributes = NULL;
    LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL;
    BOOL bInheritHandles = FALSE;
    DWORD dwCreationFlags = CREATE_NEW_CONSOLE;  // Use this flag to create a new console window
    LPVOID lpEnvironment = NULL;
    LPCTSTR lpCurrentDirectory = NULL;
    LPSTARTUPINFO lpStartupInfo = &si;
    LPPROCESS_INFORMATION lpProcessInformation = &pi;

    // Create the new process
    if (!CreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                       bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                       lpStartupInfo, lpProcessInformation)) {
        // Failed to create process
        std::cout << "[-] Failed to create process. Error code: 0x" << std::hex << GetLastError()
                  << std::endl;
    }

    int pid = pi.dwProcessId;
    return pid;
}

size_t utilities::GetHandleAddress(ULONG dwProcessId, USHORT hObject) {
    // NtQuerySystemInformation_t NtQuerySystemInformation = NULL;
    NTSTATUS(NTAPI * NtQuerySystemInformation)
    (const int SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength,
     PULONG ReturnLength) =
        (NTSTATUS(NTAPI*)(const int, PVOID, ULONG, PULONG))GetProcAddress(
            GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");

    DWORD dwHandleSize = 4096 * 16 * 16;
    BYTE* HandleInformation;
    DWORD BytesReturned;
    ULONG i;

    HandleInformation = (BYTE*)malloc(dwHandleSize);

    // Get handle information
    while (NtQuerySystemInformation(16, HandleInformation, dwHandleSize, &BytesReturned) != 0)
        HandleInformation = (BYTE*)realloc(HandleInformation, dwHandleSize *= 2);

    // Find handle
    SYSTEM_HANDLE_INFORMATION* HandleInfo = (SYSTEM_HANDLE_INFORMATION*)HandleInformation;
    PSYSTEM_HANDLE_TABLE_ENTRY_INFO CurrentHandle = &HandleInfo->Handles[0];

    for (i = 0; i < HandleInfo->HandleCount; CurrentHandle++, i++) {
        if (CurrentHandle->UniqueProcessId == dwProcessId &&
            CurrentHandle->HandleValue == hObject) {
            return (size_t)CurrentHandle->Object;
        }
    }

    return NULL;
}

}  // namespace utils
