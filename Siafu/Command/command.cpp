#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include <string>

HANDLE hReadPipe, hWritePipe;
SECURITY_ATTRIBUTES saAttr;
PROCESS_INFORMATION piProcInfo;
STARTUPINFO siStartInfo;
DWORD dwRead, dwWritten;
char buffer[1024];
BOOL bSuccess;
bool openProc = false;

void handleProc(HANDLE hReadPipe, HANDLE hWritePipe) {
    // Check if the pipe handles are valid
    if (hReadPipe == NULL || hWritePipe == NULL) {
        printf("Invalid pipe handles.\n");
        return;
    }
    
    // Read from the pipe
    char buffer[1024]; // Increased buffer size
    DWORD dwRead, dwWritten;
    BOOL bSuccess;
    bool bProcessEnded = false;
    std::string output = "";
    
    // Keep the process open indefinitely
    while (openProc) {
        // Write to the child process's standard input
        const char* message = "echo ABC123\n"; // Include newline character
        bSuccess = WriteFile(hWritePipe, message, strlen(message), &dwWritten, NULL);
        if (!bSuccess) {
            printf("WriteFile to pipe failed.\n");
            break;
        }

        // Sleep for some time before sending the next message
        Sleep(1000); // Sleep for 1 second

        // Check if the child process has ended
        bProcessEnded = WaitForSingleObject(piProcInfo.hProcess, 50) == WAIT_OBJECT_0;

        // Read from the pipe until there's no more data available
        while (true) {
            bSuccess = ReadFile(hReadPipe, buffer, sizeof(buffer), &dwRead, NULL);
            if (!bSuccess || dwRead == 0) break;
            output.append(buffer, dwRead);
        }


        // If the process has ended, break out of the loop
        if (bProcessEnded)
            break;
    }
}

void createProc() {
    HANDLE hMutex;
    // Create mutex with a name so multiple instances can detect it
    hMutex = CreateMutexA(NULL, FALSE, "ProcMutex");

    if (hMutex == NULL) {
        printf("Mutex creation failed.\n");
        return;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("BeaconMutex already exists, beacon already running\n");
        CloseHandle(hMutex);
        return;
    }

    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES saAttr;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;

    // Set the bInheritHandle flag so pipe handles are inherited
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT
    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
        printf("CreatePipe failed.\n");
        CloseHandle(hMutex);
        return;
    }

    // Ensure the write handle to the pipe for STDOUT is not inherited
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
    
    // Ensure the write handle to the pipe for STDIN is not inherited
    SetHandleInformation(hWritePipe, HANDLE_FLAG_INHERIT, 0);

    // Set up members of the STARTUPINFO structure
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdOutput = hReadPipe; // Redirect child process's standard output to the write end of the pipe
    siStartInfo.hStdInput = hWritePipe; // Pass the read end of the pipe as the child process's standard input
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Start the child process. 
    if (!CreateProcessA("C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        CloseHandle(hMutex);
        return;
    }
    openProc = TRUE;

    // Close process handles not needed in the parent process
    CloseHandle(piProcInfo.hThread);
    CloseHandle(piProcInfo.hProcess);

    printf("(+) process started! pid: %ld\n", piProcInfo.dwProcessId);
    printf("\t(+) pid: %ld, handle: 0x%x\n", piProcInfo.dwProcessId, hReadPipe);
    printf("\t(+) tid: %ld, handle: 0x%x\n\r", piProcInfo.dwThreadId, hWritePipe);

    handleProc(hReadPipe, hWritePipe);

    // Close pipe handles
    CloseHandle(hReadPipe);
    CloseHandle(hWritePipe);

    // Cleanup
    CloseHandle(hMutex);
}

int main(){
  
    createProc();

    return 0;
}

/*
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <message>\n", argv[0]);
        return 1;
    }
*/

