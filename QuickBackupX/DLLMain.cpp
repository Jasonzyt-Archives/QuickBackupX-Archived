// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

void entry();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#if defined LLSDK
        LL::registerPlugin("QuickBackupX", "A plugin to create and manage the backups",
            LL::Version::parse(QBX_VERSION), {
                {"Author", "Jasonzyt"},
                {"GitHub", "https://github.com/Jasonzyt/QuickBackupX"},
                {"Bug-report", "https://github.com/Jasonzyt/QuickBackupX/issues"}
            }
        );
#endif
        break;
    case DLL_THREAD_ATTACH: break;
    case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH: 
        break;
    }
    return TRUE;
}

extern "C" {

    __declspec(dllexport) void onPostInit() {
        entry();
    }

}

