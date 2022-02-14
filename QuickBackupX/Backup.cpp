#include "Backup.h"
#include "Utils.h"
#include "LangPack.h"

std::unordered_map<std::string, Backup> backups;

bool LocalBackup::checkDiskSpace(char disk, size_t size) {
	auto usage = getDiskUsage(disk);
	if (usage.avfree < size) {
		return false;
	}
	return true;
}

