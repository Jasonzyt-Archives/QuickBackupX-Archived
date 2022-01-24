#pragma once
#include "pch.h"
#include <nlohmann/json.hpp>

class Config {

	const nlohmann::json defaultConfig{
		{"allowCommandBlock", true},
		{"remoteBackup", false},
		{"backupOutputPath", "./backups"},
		{"remoteBackupServer", ""},
		{"autoBackupTime", {"08:00", "16:00", "00:00"}}
	};

public:

	bool allowCommandBlock = true;
	bool remoteBackup = false;
	std::string backupOutputPath;
	std::string remoteBackupServer;
	std::vector<std::string> autoBackupTime;

	void load(const std::string& cfg) {
	}

};