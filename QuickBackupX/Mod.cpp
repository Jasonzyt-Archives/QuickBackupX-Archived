#include "pch.h"
#include "Logger.h"
#include "SyncCopy.h"
#if defined(BDS_LL)
#include <HookAPI.h>
#else 
#include <loader/Loader.h>
#endif

using namespace std;

#if !defined(BDS_LL)
stdio_commit sc("[QBX] ");
OLogger<stdio_commit*> logger; // Console output
#endif
std::queue<SyncCopy> copyQueue;

void entry() {
	PRINT("QuickBackupX loaded! Author: Jasonzyt, Version: ", 
		QBX_VERSION, ", Build Time: " __DATE__ " " __TIME__);
}

THook(void, "?tick@ServerLevel@@UEAAXXZ", void* thiz) {
	if (!copyQueue.empty()) {
		copyQueue.front().tick();
		if (copyQueue.front().done()) {
			copyQueue.pop();
		}
	}
	return original(thiz);
}