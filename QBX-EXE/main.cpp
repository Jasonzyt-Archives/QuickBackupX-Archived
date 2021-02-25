#include <string>
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <TlHelp32.h>

using namespace std;

bool ProcessExists(unsigned int long pid)
{
	PROCESSENTRY32 processEntry32;
	HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (((int)toolHelp32Snapshot) != -1)
	{
		processEntry32.dwSize = sizeof(processEntry32);
		if (Process32First(toolHelp32Snapshot, &processEntry32))
		{
			do
			{
				if (pid == processEntry32.th32ProcessID)
				{
					return true;
				}
			} while (Process32Next(toolHelp32Snapshot, &processEntry32));
		}
		CloseHandle(toolHelp32Snapshot);
	}
	return false;
}

int main(int argc, char* argv[])
{
	// ./qbx.exe back [Level_Name] [BDS_PID] 
	if (string(argv[1]) == "back")
	{
		unsigned int long pid = atol(argv[3]);
		while (ProcessExists(pid))
		{
			Sleep(1000);
		}

	}
}