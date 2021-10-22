// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
using namespace QuickBackupX;
void initConsole() {
	system("chcp 65001");
	//enable colorful output
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode;
	GetConsoleMode(hOutput, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; //NOLINT
	SetConsoleMode(hOutput, dwMode);
	//std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2);
}

namespace QuickBackupX
{
	void dll_init();
	void dll_exit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		dll_init();
		initConsole();
		break;
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	case DLL_PROCESS_DETACH: 
		dll_exit();
		break;
	}
	return TRUE;
}

