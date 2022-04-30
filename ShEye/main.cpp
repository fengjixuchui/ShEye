#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include "Header.h"

DWORD getpid(const wchar_t* pname)
{
    DWORD proid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pE;
        pE.dwSize = sizeof(pE);

        if (Process32First(hSnap, &pE))
        {
            if (!pE.th32ProcessID)
                Process32Next(hSnap, &pE);
            do
            {
                if (!wcscmp(pE.szExeFile, pname))
                {
                    proid = pE.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &pE));
        }
    }
    CloseHandle(hSnap);
    return (proid);
}

int wmain()
{
    HANDLE pr = OpenProcess(PROCESS_ALL_ACCESS, 0, getpid(L"app1.exe"));
    if (!pr)
        exit(EXIT_FAILURE);
    HMODULE hm = LoadLibraryA("user32.dll");
    if (!hm)
        exit(EXIT_FAILURE);
    LPVOID add = GetProcAddress(hm, "MessageBoxA");
    if (!add)
        exit(EXIT_FAILURE);
    if (!ReadProcessMemory(pr, add, bytes, buffsz, 0))
        exit(EXIT_FAILURE);
    for (int i = 0; i < buffsz; i++)
    {
        if (bytes[i] == 0x48 || bytes[i] == 0xFF )
        {
            if (bytes[i + 1] == 0xB8 || bytes[i + 1] == 0xE0)
            {
                MessageBoxA(0, "hook detected", 0, 0);
                exit(-4);
            }
        }
    }
    MessageBoxA(0, "no hook detected in this process", 0, 0);
}

