#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>



void createProc()
{
    // info.lpFile = L"cmd.exe";
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    DWORD TID = pi.dwThreadId;
    DWORD PID = pi.dwProcessId;
    HANDLE hThread = pi.hThread;
    HANDLE hProcess = pi.hProcess;

    // LPTSTR cmdArgs = " name@example.com";   
    // Start the child process. 
    if( !CreateProcessA( "C:\\Windows\\System32\\cmd.exe",   // No module name (use command line)
        NULL,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    printf("\t(+) pid: %ld, handle: 0x%x\n", PID, hProcess);
    printf("\t(+) tid: %ld, handle: 0x%x\n\r", TID, hThread);
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

int main(){
  
    createProc();

    return 0;
}