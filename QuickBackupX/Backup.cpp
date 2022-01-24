#include "Backup.h"
#include "Utils.h"
#include "LangPack.h"

bool LocalBackup::checkDiskSpace(char disk, size_t size) {
	auto usage = getDiskUsage(disk);
	if (usage.avfree < size) {
		PRINT<WARN, YELLOW>();
	}
}