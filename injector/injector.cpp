#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include "../shared.h"


/**
* Injects DLL from given path into a certain process
* 
* @param pid Target process ID
* @param dll_path Target DLL's exact path
* @returns True, if successfully injected. False, otherwise (can return true if DLL path is invalid)
*/
bool inject(DWORD pid, const char* dll_path)
{
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (handle)
    {
        LPVOID lp_dll_path = VirtualAllocEx(handle, 0, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);
        SIZE_T size = strlen(dll_path) + 1;
        WriteProcessMemory(handle, lp_dll_path, (LPVOID)dll_path, size, 0);

        HANDLE thread_handle = CreateRemoteThread(handle, 0, 0,
            (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"), lp_dll_path, 0, 0);

        WaitForSingleObject(thread_handle, INFINITE);
        VirtualFreeEx(handle, lp_dll_path, size, MEM_RELEASE);
        printf("Injected.");
        return true;
    }
    std::cout << "Failed to inject with error code 0x" << std::hex << GetLastError();
    return false;
}

/**
* Compile lib/Run.java -> lib/Run.class with javac
* 
* @param dir Program dir
*/
void try_compile_run(std::string dir) {
    std::string path = dir + "\\lib\\Run.java";
    std::string cmd = "javac " + path;
    system(cmd.c_str());
}


int main(int argc, char* argv[])
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // Find JVM process
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    std::string dir = get_dir(argv[0]);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            // Found JVM process
            if (_wcsicmp(entry.szExeFile, L"javaw.exe") == 0)
            {
                // Try compile Run.java -> Run.class
                try_compile_run(dir);
                // Make full path to lib.dll (based on run path)
                std::string full_path;
                (full_path += dir) += "\\lib.dll";
                // Inject DLL into JVM process ID
                bool ret = inject(entry.th32ProcessID, full_path.c_str());
                CloseHandle(snapshot);
                goto end;
            }
        }
    }
    printf("JVM process not found.");
    end:
    return getchar();
}