#pragma once

#ifndef _UTILS_H
#define _UTILS_H

#include <windows.h>

#include <psapi.h>

#include <cstdint>
#include <iomanip>
#include <vector>

namespace utils {
constexpr int const SystemModuleInformation = 0xb;
constexpr int const SystemHandleInformation = 16;

class utilities {
public:
    uint64_t GetKernelBaseAddress();
    void dumpBuffer(void* ptr, size_t size);
    size_t GetHandleAddress(ULONG dwProcessId, USHORT hObject);
    int CreateProcessWrapper(LPCTSTR lpApplicationName);
};

}  // namespace utils

#endif
