#include "pch.h"
#include "Backup.h"
#include "Utils.h"
#include "LangPack.h"
#include "ZipArchive.h"

std::unordered_map<std::string, LocalBackup> backups;

bool LocalBackup::checkDiskSpace(char disk, size_t size) {
	auto usage = getDiskUsage(disk);
	if (usage.avfree < size) {
		return false;
	}
	return true;
}

bool LocalBackup::packZip(const std::string& path, const std::string& dst) {
	try {
		ZipArchive zip;
		zip.open(dst);
		if (!zip.isOpen()) {
			return false;
		}
		zip.addFolder(path);
		zip.close();
		zip.save();
		zip.release();
	}
	catch (std::exception& e) {
		logger.error("Failed to pack zip: %s", e.what());
		return false;
	}
	catch (...) {
		return false;
	}
	return true;
}