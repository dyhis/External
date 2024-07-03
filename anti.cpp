//#include <windows.h>
//#include <iostream>
//#include "stdint.h"
//#include "anti.h"
//#include "../qengine/engine/qengine.hpp"
//
//typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
//typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
//
//void anti::bsod() 
//{
//    BOOLEAN bEnabled;
//    ULONG uResp;
//    LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
//    LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtRaiseHardError");
//    pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
//    pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
//    NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
//    NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
//    Sleep(5000);
//    std::cout << ("Triggered BSOD") << std::endl;
//    Sleep(1000);
//    exit(0);
//}
//
//DWORD WINAPI MetrickDummy(LPVOID lpParam) 
//{
//    while (true) {
//        Sleep(0);
//    }
//    return 0;
//}
//void anti::anti_debugging()
//{
//
//    HANDLE dummyThread = CreateThread(NULL, NULL, MetrickDummy, NULL, NULL, NULL);
//
//    if (ResumeThread(dummyThread) >= 1) 
//    {
//        //crash process
//        anti::bsod();
//        for (long long int i = 0; ++i; (&i)[i] = i);
//        *((char*)NULL) = 0;
//    }
//
//    Sleep(1);
//}
//void anti::anti_attach()
//{
//    HMODULE hNtdll = GetModuleHandleA("\x6E\x74\x64\x6C\x6C\x2E\x64\x6C\x6C");
//    if (!hNtdll)
//        return;
//
//    FARPROC pDbgBreakPoint = GetProcAddress(hNtdll, "DbgBreakPoint");
//    if (!pDbgBreakPoint)
//        return;
//
//    DWORD dwOldProtect;
//    if (!VirtualProtect(pDbgBreakPoint, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
//        return;
//
//    *(PBYTE)pDbgBreakPoint = (BYTE)0xC3;
//}
