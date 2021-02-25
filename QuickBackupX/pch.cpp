// Created by JasonZYT on 2021/02/03
#include "pch.h"
#include <fstream>
#include <iomanip>

namespace QuickBackupX
{
	void dll_init();
	void dll_exit();
	void initConsole() {
		system("chcp 65001");
		//enable colorful output
		HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode;
		GetConsoleMode(hOutput, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; //NOLINT
		SetConsoleMode(hOutput, dwMode);
		std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2);
	}
}

// DLL模块启动或退出时候调用
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: break;//QuickBackupX::init();
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	case DLL_PROCESS_DETACH: QuickBackupX::dll_exit(); break;
	}
	return TRUE;
}

extern "C" {
	_declspec(dllexport) void onPostInit() {
		std::ios::sync_with_stdio(false);
		QuickBackupX::dll_init();
		QuickBackupX::initConsole();
	}
}
