#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

HANDLE hPipe;
HANDLE hReadPipe, hWritePipe;
SECURITY_ATTRIBUTES saAttr;
PROCESS_INFORMATION piProcInfo;
STARTUPINFO siStartInfo;
DWORD dwRead, dwWritten;
char buffer[1024];
BOOL bSuccess;

void handleProc() {

    // Create a pipe for the child process's STDOUT
    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
        printf("CreatePipe failed.\n");
        return;
    }

        // Keep the process open indefinitely
    while (true) {
        // Write to the child process's standard input
        const char* message = "echo ABC123";
        bSuccess = WriteFile(hWritePipe, message, strlen(message), &dwWritten, NULL);
        if (!bSuccess) {
            printf("WriteFile to pipe failed.\n");
            break;
        }

        // Sleep for some time before sending the next message
        Sleep(1000); // Sleep for 1 second

        // Read from the pipe
        bSuccess = ReadFile(hReadPipe, buffer, sizeof(buffer), &dwRead, NULL);
        if (!bSuccess || dwRead == 0) {
            printf("ReadFile from pipe failed.\n");
            return;
        }

        printf("Received from child process: %s\n", buffer);

    }

    // Close handles
    CloseHandle(hReadPipe);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

}

void createProc()
{
    HANDLE hMutex;
    // create mutex with a name so multiple instances can detect it
    hMutex = CreateMutexA(NULL, FALSE, "ProcMutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // if this process created the mutex, exit the application
        if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("BeaconMutex already exists, beacon already running\n");
        CloseHandle(hMutex);
        return;
        }
    }

    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES saAttr;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    DWORD dwRead, dwWritten;
    char buffer[100];
    BOOL bSuccess;

    // Set the bInheritHandle flag so pipe handles are inherited
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT
    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
        printf("CreatePipe failed.\n");
        return;
    }

    // Ensure the write handle to the pipe for STDOUT is not inherited
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
    
    // Ensure the write handle to the pipe for STDIN is not inherited
    SetHandleInformation(hWritePipe, HANDLE_FLAG_INHERIT, 0);

    // Set up members of the PROCESS_INFORMATION structure
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdOutput = hWritePipe; // Redirect child process's standard output to the write end of the pipe
    siStartInfo.hStdInput = hReadPipe; // Pass the read end of the pipe as the child process's standard input
    
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

 
    // Start the child process. 
    if( !CreateProcessA( "C:\\Windows\\System32\\cmd.exe",   // No module name (use command line)
        NULL,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &siStartInfo,            // Pointer to STARTUPINFO structure
        &piProcInfo )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    DWORD TID = piProcInfo.dwThreadId;
    DWORD PID = piProcInfo.dwProcessId;
    HANDLE hThread = piProcInfo.hThread;
    HANDLE hProcess = piProcInfo.hProcess;

    printf("(+) process started! pid: %ld\n", PID);
    printf("\t(+) pid: %ld, handle: 0x%x\n", PID, hProcess);
    printf("\t(+) tid: %ld, handle: 0x%x\n\r", TID, hThread);

    handleProc();

    // Close process and thread handles. 
    CloseHandle( piProcInfo.hProcess );
    CloseHandle( piProcInfo.hThread );

    // cleanup
    if (hMutex)
        CloseHandle(hMutex);
    return;

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