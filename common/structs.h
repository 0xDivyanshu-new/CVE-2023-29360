#pragma once

#ifndef COMMON_STRUCTS_H_
#define COMMON_STRUCTS_H_

#include <Windows.h>

#define NTSTATUS LONG

struct SYSTEM_MODULE {
    ULONG64 Reserved1;
    ULONG64 Reserved2;
    PVOID Base;
    ULONG Size;
    ULONG Flags;
    USHORT Index;
    USHORT Unknown;
    USHORT LoadCount;
    USHORT ModuleNameOffset;
    CHAR ImageName[256];
};
using PSYSTEM_MODULE = SYSTEM_MODULE*;

struct SYSTEM_MODULE_INFORMATION {
    ULONG ModulesCount;
    SYSTEM_MODULE Modules[1];
};

struct SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    unsigned short UniqueProcessId;
    unsigned short CreatorBackTraceIndex;
    unsigned char ObjectTypeIndex;
    unsigned char HandleAttributes;
    unsigned short HandleValue;
    void* Object;
    unsigned long GrantedAccess;
    long __PADDING__[1];
};

using PSYSTEM_HANDLE_TABLE_ENTRY_INFO = SYSTEM_HANDLE_TABLE_ENTRY_INFO*;

struct SYSTEM_HANDLE_INFORMATION {
    ULONG HandleCount;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
};
using PSYSTEM_HANDLE_INFORMATION = SYSTEM_HANDLE_INFORMATION;

using PSYSTEM_MODULE_INFORMATION = SYSTEM_MODULE_INFORMATION*;

using NtQuerySystemInformation_t = NTSTATUS(NTAPI*)(_In_ ULONG SystemInformationClass,
                                                    _In_ PVOID SystemInformation,
                                                    _In_ ULONG SystemInformationLength,
                                                    _In_ PULONG ReturnLength);

#endif  // COMMON_STRUCTS_H_
