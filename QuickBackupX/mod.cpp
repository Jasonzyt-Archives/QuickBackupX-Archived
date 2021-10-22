#include "pch.h"
#include "logger.h"

using namespace std;

namespace QuickBackupX
{
	stdio_commit sc("[QBX] ");
	file_commit fc(QBX_LOGFILE.c_str());
	Logger<stdio_commit*> coutp(&sc, true); // Console output
	Logger<file_commit*> loutp(&fc, false,"[%H:%M:%S"); // Log output
	void dll_init()
	{
		PRINT("QuickBackupX loaded! Author: Jasonzyt");

	}
	void dll_exit(){}
}